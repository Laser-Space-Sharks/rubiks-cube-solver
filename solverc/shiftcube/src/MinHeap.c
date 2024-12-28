#include "servoCoder.h"
#include "inter_move_table.h"

typedef struct MinHeapMap {
    size_t length;
    size_t size;
    MinHeapNode* nodes;
} MinHeapMap;

typedef struct MinHeapMap_insertMessage {
    MinHeapNode* ptr;
    bool NodeIsNew;
} MinHeapMap_insertMessage;

/*
typedef struct MinHeapNode {
    State_s state;
    int algorithm_index;
    bool isBefore;
    double weight;
    struct MinHeapNode* parent;
} MinHeapNode;
*/

MinHeapMap* MinHeapMap_create(size_t size) {
    MinHeapMap* map = (MinHeapMap*)malloc(sizeof(MinHeapMap));
    map->length = 0;
    map->size = size;

    map->nodes = (MinHeapNode*)calloc(size, sizeof(MinHeapNode));

    return map;
}

size_t MinHeapMap_hash(const MinHeapMap* map, const MinHeapNode* key) {
    // Hash could be any of 7776*N + 1 different numbers.
    if (State_is_ROBOT_START_STATE(&(key->state)) && key->algorithm_index == -1) return 0;
    size_t hash = 0;
    hash += key->algorithm_index;
    hash *= 2;
    hash |= key->isBefore;
    hash *= 24;
    hash += (key->state.persp.face)*4 + key->state.persp.rot;
    hash *= 162;
    hash += inter_move_table_hash(&(key->state.servos));
    return (hash % (map->size - 1)) + 1;
}
MinHeapMap_insertMessage MinHeapMap_insert(MinHeapMap *map, const MinHeapNode* key) {

    size_t hash = MinHeapMap_hash(map, key);
    size_t index = hash;

    while (!compare_states(&(map->nodes[index].state), &(NULL_STATE))) {
        if (compare_MinHeapNodes(&(map->nodes[index]), key)) {
            map->nodes[index].weight = key->weight;
            map->nodes[index].parent = key->parent;

            return (MinHeapMap_insertMessage) {
                .ptr = &(map->nodes[index]),
                .NodeIsNew = false
            };
        }

        if (index + 1 == hash) {
            break;
        }

        index = (index + 1 >= map->size) ? 0 : index + 1;
    }

    map->nodes[index] = *key;
    return (MinHeapMap_insertMessage) {
        .ptr = &(map->nodes[index]),
        .NodeIsNew = true
    };
}

void MinHeapMap_free(MinHeapMap *map) {
    if (map == NULL || map->nodes == NULL) {
        free(map);
        return;
    };
    free(map->nodes);
    free(map);
}


typedef struct MinHeapNode {
    State_s state;
    size_t algorithm_index;
    bool isBefore;
    double weight;
    struct MinHeapNode* parent;
    size_t heapIndex;
} MinHeapNode;

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

MinHeap* MinHeap_create(size_t N) {
    size_t size = 960*N + 1;
    MinHeap* minheap = (MinHeap*)malloc(sizeof(MinHeap));
    minheap->heap = (MinHeapNode**)calloc(size, sizeof(MinHeapNode*));
    minheap->nodes_to_indexes = MinHeapMap_create(size*2);
    minheap->size = 0;
    minheap->capacity = size;
}
static inline size_t parent_index(size_t i) {
    return (i - 1) >> 1;
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
    // save minnode
    MinHeapNode* minnode = minheap->heap[0];
    // move last into root
    MinHeapNode* curr_node = minheap->heap[minheap->size-1];
    minheap->heap[minheap->size-1] = NULL;
    minheap->size--;

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
bool MinHeap_update_key(MinHeap* minheap, const State_s* state, size_t algorithm_index, bool isBefore, double weight, MinHeapNode* parent) {
    MinHeapNode query = {
        .state = *state,
        .algorithm_index = algorithm_index,
        .isBefore = isBefore,
        .weight = weight,
        .parent = parent,
        .heapIndex = minheap->size
    };
    MinHeapMap_insertMessage message = MinHeapMap_insert(minheap->nodes_to_indexes, &query);
    MinHeapNode* node = message.ptr; // This ptr points to the actual node stored the map.
    bool nodeWasNew = message.NodeIsNew;

    if (nodeWasNew) { // If node was new, we need to insert
        if (minheap->size == minheap->capacity) return false;
        minheap->heap[node->heapIndex] = node;
        minheap->size++;
    } MinHeap_bubble_up(minheap, node->heapIndex);
    return true;
}
void MinHeap_free(MinHeap* minheap) {
    if (minheap == NULL) return;
    free(minheap->heap);
    MinHeapMap_free(minheap->nodes_to_indexes);
    free(minheap);
}