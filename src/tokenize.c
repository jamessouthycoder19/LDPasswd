#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ldpasswd/leet.h"
#include "ldpasswd/data.h"
#include "ldpasswd/tokenize.h"
#include "ldpasswd/data_helpers.h"

/**
 * Searches the trie for a prefix of 'pw'.
 * Fills 'frequencies' array with the frequency found at each depth.
 * 'frequencies' should be pre-allocated to at least strlen(pw).
 */
void find_word(const char *pw, int* last_valid_depth, float *costs) {
    // current_idx is the index in the trie array where we start (i.e. the root)
    int current_idx = 0;
    // current_cnt is the number of nodes at the current level (initially the root level, so 26 for the characters in the alphabet)
    int current_cnt = 26;
    
    // table is the entire trie loaded into memory as an array of Nodes
    Node *table = (Node *)___data_bin;

    // Password Length
    int pw_len = strlen(pw);
    
    int current_depth = 0;

    // Initialize costs array to 0
    for (int i = 0; i < pw_len; i++) costs[i] = 0;

    while (current_depth < pw_len) {
        Node key = {0};
        key.token = pw[current_depth];

        // Binary search for the current character among the current node's children
        Node *match = (Node *)bsearch(&key, &table[current_idx], current_cnt, sizeof(Node), compare_nodes);

        if (match) {
            // Record the frequency at this specific depth
            // If it's not a word, the generator set this to 0 already
            if (match->is_word) {
                costs[current_depth] = match->cost;
                *last_valid_depth = current_depth + 1; // Length of the word found so far
            } else {
                costs[current_depth] = 0;
            }

            // Prepare for next iteration
            current_depth++;

            // If no children exist, stop searching
            if (match->child_idx == -1 || match->child_cnt == 0) {
                break;
            }

            // Move search range to this node's children
            current_idx = match->child_idx;
            current_cnt = match->child_cnt;
        } else {
            // No matching character found; trie path ends here
            break;
        }
    }
}


/**
 * Tokenizes the password into an array of tokens, where each token is either a word, a number, or a special character.
 * The start indices of each token in the original password are stored in the start_of_token_indicies array, 
 * and the type of each token is stored in the token_types array (word, number, special, or other).
 */
int tokenize_password(char *pw, int *start_of_token_indices, char *token_types, char *unleeted_pw) {
    int pw_length = strlen(pw);
    int token_count = 0;

    // Handle Un-leeting
    // the genreate_unleet function returns an array of all possible un-leeted versions of the password, 
    // we will use to find potential word matches in the trie.
    char* lower_password = strdup(pw);
    for (int j = 0; lower_password[j]; j++) lower_password[j] = tolower(lower_password[j]);
    ResultList results = generate_unleet(lower_password);
    free(lower_password);

    // Iterate through the password
    for (int i = 0; i < pw_length; ) {
        if (token_count >= 20) break;

        int best_len = 0;
        
        // Start with very high costs
        float best_total_path_cost = 9999.0f;
        float best_word_cost = 9999.0f; 
        
        // We need a split penalty to prevent "P"+"a"+"s" being cheaper than "Pas"
        float split_penalty = 5.0f; 

        // Variable to temporarily store the unleeted word used to find the lowest cost
        char *temp_best_unleeted_ptr = NULL;
        
        /**
         * Start by finding the best possible word starting at index i.
         * We check all un-leeted versions of the password to find potential matches in the trie.
         * For each motential match, whatever word would come next is also considered.
         * The lowest sum of costs of the current word + next word + split penalty is chosen as the best path forward.
         */
        for (int j = 0; j < results.count; j++) {
            float costs[128] = {0};
            int last_depth = 0;

            // Find the longest word starting at index i for this un-leeted version of the password, and the cost at each depth
            find_word(results.strings[j] + i, &last_depth, costs);

            // Iterate through each valid word depth (ex. 'password' will check 'pass', then 'password' (and probobly something else in between))
            for (int len = 1; len <= last_depth; len++) {
                if (costs[len - 1] > 0) {
                    // Cost of current word
                    float word1_cost = costs[len - 1];
                    
                    // Legitimacy check for short words
                    if (len <= 2 && !isalpha(pw[i])) continue;

                    // Now, find the best possible word that could follow this one, starting at index i + len

                    // Penalty if nothing follows, this prevents the algorithm from favoring paths that split off 
                    // a valid word and then have a long tail of garbage that doesn't match anything in the trie 
                    float best_follow_up_cost = 15.0f;
                    
                    for (int k = 0; k < results.count; k++) {
                        float next_costs[128] = {0};
                        int next_depth = 0;
                        if (i + len < pw_length) {
                            find_word(results.strings[k] + i + len, &next_depth, next_costs);
                            float c = next_costs[next_depth - 1];

                            // If this path has a valid follow-up word and it's cheaper than 
                            // the best follow-up cost we've found so far, update it
                            if (c < best_follow_up_cost) {
                                best_follow_up_cost = c;
                            }
                        } else {
                            best_follow_up_cost = 0; // End of string is "free"
                        }
                    }

                    // Total Path = current word + next word + penalty for having two tokens
                    float total_path_cost = word1_cost + best_follow_up_cost + split_penalty;

                    if (total_path_cost < best_total_path_cost) {
                        best_total_path_cost = total_path_cost;
                        best_len = len;
                        best_word_cost = word1_cost;
                        temp_best_unleeted_ptr = results.strings[j] + i;
                    }
                }
            }
        }

        // Now, just assign the tokens, and bump the value of i appropriately
        start_of_token_indices[token_count] = i;

        int is_word = 0;
        if (best_len > 2) {
            for (int k = 0; k < best_len; k++) {
                if (isalpha(pw[i + k])) {
                    is_word = 1;
                    break;
                }
            }
        } else {
            is_word = 1;
        }

        if (best_len > 0 && (best_len > 1 || isalpha(pw[i])) && is_word == 1) {
            token_types[token_count] = 'w';
            // if for some reason temp_best_unleeted_ptr is NULL, fallback to original
            const char *src = temp_best_unleeted_ptr ? temp_best_unleeted_ptr : (pw + i);
            memcpy(unleeted_pw + i, src, best_len);
            i += best_len;
        } 
        else if (isdigit(pw[i])) {
            // If we find a number, it'll keep looking for the next number, to keep them all together as 1 token
            // j variable makes sure we don't go past the maximum number in c
            int j = 0;
            token_types[token_count] = 'n';
            while (i < pw_length && isdigit(pw[i]) && j < 6) {
                unleeted_pw[i] = pw[i];
                i++;
                j++;
            }
        }
        else if (ispunct(pw[i])) {
            token_types[token_count] = 's';
            unleeted_pw[i] = pw[i];
            i++;
        }
        else {
            token_types[token_count] = 'o';
            unleeted_pw[i] = pw[i];
            i++;
        }
        token_count++;
    }

    // Cleanup
    for (int i = 0; i < results.count; i++) free(results.strings[i]);
    free(results.strings);
    return token_count;
}