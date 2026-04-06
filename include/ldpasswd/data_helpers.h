#ifndef LDPASSWD_DATA_HELPERS_H
#define LDPASSWD_DATA_HELPERS_H

#include <stdint.h>

// This struct holds information for each leaf in the trie.
// The trie is flattened into an array of these nodes for efficient 
// binary search (this occurs in the python script create-trie.py).
#pragma pack(push, 1)
typedef struct Node {
    char token;          // 1 byte - this is the character of the node
    char is_word;        // 1 byte - whether or not this is a valid last character of a word
    uint8_t _pad[2];     // 2 bytes padding - padding for the compiler :(
    float cost;          // 4 bytes (The entropy bits) - cost of the word (-log2(frequency of word/total_frequency))
    int32_t child_idx;   // 4 bytes - index in the array where this node's children start (-1 if no children)
    int32_t child_cnt;   // 4 bytes - number of children nodes (0 if no children)
} Node;
#pragma pack(pop)

int compare_nodes(const void *a, const void *b);

#endif