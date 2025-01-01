#ifndef MINHEAP_H
#define MINHEAP_H

#include "servoCoder.h"

typedef struct MinHeapNode {
    State_s state; // 4 bytes
    int8_t algorithm_index; // 1 bytes
    bool isBefore; // 1 byte
    float weight; // 4 bytes
    struct MinHeapNode* parent; // 4 on 32-bit and 8 on 64-bit
    uint16_t heapIndex; // 2 bytes
} MinHeapNode; // 16 on 32-bit and 20 on 64-bit

bool compare_MinHeapNodes(const MinHeapNode* node1, const MinHeapNode* node2);
typedef struct MinHeap MinHeap;
void print_MinHeapNode(const MinHeapNode* node);
MinHeap* MinHeap_create(MovePair* alg_sections, size_t numAlgSecs);
MinHeapNode* MinHeap_pluck_min(MinHeap* minheap);
void MinHeap_update_key(MinHeap* minheap, const State_s* state, int8_t algorithm_index, bool isBefore, float weight, MinHeapNode* parent);
void MinHeap_free(MinHeap* minheap);

#endif // MINHEAP_H