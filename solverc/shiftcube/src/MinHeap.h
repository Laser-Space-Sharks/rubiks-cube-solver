#ifndef MINHEAP_H
#define MINHEAP_H

#include "servoCoder.h"

typedef struct MinHeapNode {
    State_s state;
    int algorithm_index;
    bool isBefore;
    double weight;
    struct MinHeapNode* parent;
    size_t heapIndex;
} MinHeapNode;

bool compare_MinHeapNodes(const MinHeapNode* node1, const MinHeapNode* node2);
typedef struct MinHeap MinHeap;
void print_MinHeapNode(const MinHeapNode* node);
MinHeap* MinHeap_create(size_t numSingleMoves, size_t numOppPairs);
MinHeapNode* MinHeap_pluck_min(MinHeap* minheap);
bool MinHeap_update_key(MinHeap* minheap, const State_s* state, int algorithm_index, bool isBefore, double weight, MinHeapNode* parent);
void MinHeap_free(MinHeap* minheap);

#endif // MINHEAP_H