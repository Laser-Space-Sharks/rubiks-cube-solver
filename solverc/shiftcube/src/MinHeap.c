#include "MinHeap.h"
#include "servoCoder.h"
#include "move.h"

typedef struct MinHeapMap {
    size_t length;
    size_t size;
    face_e* movefaces;
    int8_t numMoves;
    MinHeapNode* nodes;
} MinHeapMap;
typedef struct MinHeapMap_insertMessage {
    MinHeapNode* ptr;
    bool NodeIsNew;
} MinHeapMap_insertMessage;

static const MinHeapNode TEST_CASE = {
    // (State: ((U, 0), n.E.S.W.U0.R2.D1.L2), algInd: 0, isBefore: 0)
    .state = {
        .persp = {
            .face = FACE_U,
            .rot = 0
        },
        .servos = {
            0,1,1,1,0,2,1,2
        }
    },
    .algorithm_index = 0,
    .isBefore = 0,
    .weight = 0,
    .parent = NULL,
    .heapIndex = 0
};

/*
typedef struct MinHeapNode {
    State_s state;
    int8_t algorithm_index;
    bool isBefore;
    float weight;
    struct MinHeapNode* parent;
} MinHeapNode;
*/

MinHeapMap* MinHeapMap_create(MovePair* alg_sections, size_t numAlgSecs) {
    MinHeapMap* map = (MinHeapMap*)malloc(sizeof(MinHeapMap));
    map->numMoves = numAlgSecs;
    map->movefaces = (face_e*)malloc(numAlgSecs*sizeof(face_e));
    for (int i = 0; i < numAlgSecs; i++) {
        map->movefaces[i] = alg_sections[i].move1.face;
    }


    size_t size = 5184*numAlgSecs + 1;

    map->length = 0;
    map->size = size;

    map->nodes = (MinHeapNode*)calloc(size, sizeof(MinHeapNode));

    return map;
}

size_t MinHeapMap_hash(const MinHeapMap* map, const MinHeapNode* key) {
    // Hash could be any of (2*16*162*N + 1 = 5184*N + 1) different numbers.
    if (key->algorithm_index == -1) return 0;
    size_t hashface = key->state.persp.face;
    hashface -= ((hashface > map->movefaces[key->algorithm_index]) + (hashface > opposite_faces[map->movefaces[key->algorithm_index]]));

    size_t hash = 0;
    hash += key->algorithm_index;
    hash *= 2;
    hash += key->isBefore;
    hash *= 16;
    hash += (hashface)*4 + key->state.persp.rot;
    hash *= 162;
    hash += inter_move_table_hash(&(key->state.servos)) + 1;
    //return (hash % (map->size - 1)) + 1;
    return hash;
}
MinHeapMap_insertMessage MinHeapMap_insert(MinHeapMap *map, const MinHeapNode* key) {
    size_t hash = MinHeapMap_hash(map, key);
    if (map->nodes[hash].algorithm_index == 0 && map->nodes[hash].parent == NULL) {
        map->nodes[hash] = *key;
        return (MinHeapMap_insertMessage) {
            .ptr = &(map->nodes[hash]),
            .NodeIsNew = true
        };
    } else {
        if (map->nodes[hash].weight > key->weight) {
            map->nodes[hash].weight = key->weight;
            map->nodes[hash].parent = key->parent;
        }
        return (MinHeapMap_insertMessage) {
            .ptr = &(map->nodes[hash]),
            .NodeIsNew = false
        };
    }
}

void MinHeapMap_free(MinHeapMap *map) {
    if (map == NULL || map->nodes == NULL) {
        free(map);
        return;
    };
    free(map->nodes);
    free(map->movefaces);
    free(map);
}

bool compare_MinHeapNodes(const MinHeapNode* node1, const MinHeapNode* node2) {
    return (
        compare_states(&(node1->state), &(node2->state)) &&
        node1->algorithm_index == node2->algorithm_index &&
        node1->isBefore == node2->isBefore
    );
}

typedef struct MinHeap {
    MinHeapNode** heap; // Array of pointers to MinHeapNodes stored in *nodes_to_indexes.
    MinHeapMap* nodes_to_indexes;
    size_t size;
    size_t capacity;
} MinHeap;

void print_MinHeapNode(const MinHeapNode* node) {
    if (node->parent == NULL) {
        printf("("); print_State(node->state);
        printf(", algInd: %d, isBefore: %hhu), weight=%lf, parent=NULL\n",
            node->algorithm_index,
            node->isBefore,
            node->weight
        );
    } else {
        printf("(");
        print_State(node->state);
        printf(", algInd: %d, isBefore: %hhu), weight=%lf, parent=(",
            node->algorithm_index,
            node->isBefore,
            node->weight
        );
        print_State(node->parent->state);
        printf(", algInd: %d, isBefore: %hhu)\n", 
            node->parent->algorithm_index,
            node->parent->isBefore
        );
    }
}

MinHeap* MinHeap_create(MovePair* alg_sections, size_t numAlgSecs) {
    size_t numSingleMoves = 0;
    size_t numOppPairs = 0;
    for (size_t i = 0; i < numAlgSecs; i++) {
        if (MovePair_is_singleMove(alg_sections[i])) {
            numSingleMoves++;
        } else numOppPairs++;
    }
    size_t size = (480*2*numSingleMoves) + (256*2*numOppPairs) + 1;
    MinHeap* minheap = (MinHeap*)malloc(sizeof(MinHeap));
    minheap->heap = (MinHeapNode**)calloc(size, sizeof(MinHeapNode*));
    minheap->nodes_to_indexes = MinHeapMap_create(alg_sections, numAlgSecs);
    minheap->size = 0;
    minheap->capacity = size;
    return minheap;
}
static inline size_t parent_index(size_t i) {
    return ((i - 1) >> 1);
}
static inline size_t left_child_index(size_t i) {
    return ((2 * i) + 1);
}
static inline size_t right_child_index(size_t i) {
    return (2 * (i + 1));
}
void MinHeap_bubble_up(MinHeap* minheap, size_t curr_index) {
    MinHeapNode* this_node = minheap->heap[curr_index];
    while (curr_index > 0) {
        size_t parent_ind = parent_index(curr_index);
        if (minheap->heap[parent_ind]->weight > this_node->weight) {
            minheap->heap[parent_ind]->heapIndex = curr_index;
            minheap->heap[curr_index] = minheap->heap[parent_ind];
            curr_index = parent_ind;
        } else break;
    }
    this_node->heapIndex = curr_index;
    minheap->heap[curr_index] = this_node;
}
MinHeapNode* MinHeap_pluck_min(MinHeap* minheap) {
    //printf("This plucking time... minheap size was (%zu,%zu), and min was: ", minheap->size, minheap->SupposedSize);
    //print_MinHeapNode(minheap->heap[0]);

    // save minnode
    MinHeapNode* minnode = minheap->heap[0];
    // move last into root
    MinHeapNode* curr_node = minheap->heap[minheap->size-1];
    minheap->heap[minheap->size-1] = NULL;
    minheap->size--;
    if (minheap->size == 0) return minnode;


    // bubble down
    size_t curr_index = 0;
    size_t left_child_ind = left_child_index(curr_index);
    size_t right_child_ind = right_child_index(curr_index);
    while (left_child_ind < minheap->size) {
        size_t smaller_child_ind = left_child_ind;
        if (right_child_ind < minheap->size && minheap->heap[right_child_ind]->weight < minheap->heap[left_child_ind]->weight) {
            smaller_child_ind = right_child_ind;
        }
        if (minheap->heap[smaller_child_ind]->weight < curr_node->weight) {
            minheap->heap[smaller_child_ind]->heapIndex = curr_index;
            minheap->heap[curr_index] = minheap->heap[smaller_child_ind];
            curr_index = smaller_child_ind;
            left_child_ind = left_child_index(curr_index);
            right_child_ind = right_child_index(curr_index);
        } else break;
    }
    minheap->heap[curr_index] = curr_node;
    curr_node->heapIndex = curr_index;

    return minnode;
}
void MinHeap_update_key(MinHeap* minheap, const State_s* state, int8_t algorithm_index, bool isBefore, float weight, MinHeapNode* parent) {
    MinHeapNode query = {
        .state = *state,
        .algorithm_index = algorithm_index,
        .isBefore = isBefore,
        .weight = weight,
        .parent = parent,
        .heapIndex = minheap->size
    };
    //printf("Entering MinHeapMap_insert()\n");
    MinHeapMap_insertMessage message = MinHeapMap_insert(minheap->nodes_to_indexes, &query);
    //printf("Finished MinHeapMap_insert()\n");
    MinHeapNode* node = message.ptr; // This ptr points to the actual node stored the map.
    bool nodeWasNew = message.NodeIsNew;
    //printf("insert()'s message was:\n");
    //printf("\tptr: %zu\n", node);
    //printf("\nodeIsNew: %hhu\n", nodeWasNew);

    if (nodeWasNew) { // If node was new, we need to insert
        minheap->size++;
        minheap->heap[node->heapIndex] = node;
        MinHeap_bubble_up(minheap, node->heapIndex);
    } // The condition we don't like here, is where we re-encounter a node that we already plucked as min.
    else if (!(minheap->heap[0] == node && node->heapIndex == 0)) {
        MinHeap_bubble_up(minheap, node->heapIndex);
    }
    return;
}
void MinHeap_free(MinHeap* minheap) {
    if (minheap == NULL) return;
    free(minheap->heap);
    MinHeapMap_free(minheap->nodes_to_indexes);
    free(minheap);
}