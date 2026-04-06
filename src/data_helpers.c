#include "ldpasswd/data_helpers.h"

int compare_nodes(const void *a, const void *b) {
    Node *nodeA = (Node *)a;
    Node *nodeB = (Node *)b;
    if (nodeA->token < nodeB->token) return -1;
    if (nodeA->token > nodeB->token) return 1;
    return 0;
}