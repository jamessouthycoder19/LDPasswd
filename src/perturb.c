#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <time.h>

#include "ldpasswd/data.h"
#include "ldpasswd/data_helpers.h"

int exponential_mechanism(double *utility_space, int utility_space_length, double budget) {
    double *weights = malloc((utility_space_length) * sizeof(double));
    double total_weight = 0;
    for (int i = 0; i < utility_space_length; i++) {
        weights[i] = exp((utility_space[i] * budget) / 2.0);
        total_weight += weights[i];
    }

    double *probabiltiies = malloc((utility_space_length) * sizeof(double));
    for (int i = 0; i < utility_space_length; i++) {
        probabiltiies[i] = weights[i] / total_weight;
    }

    double rand_val = ((double) rand() / (double) RAND_MAX);
    double cumulative_prob = 0.0;

    for (int i = 0; i < utility_space_length; i++) {
        cumulative_prob += probabiltiies[i];
        if (rand_val < cumulative_prob) {
            free(weights);
            free(probabiltiies);
            return i;
        }
    }

    free(weights);
    free(probabiltiies);
    return utility_space_length - 1;
}

char* perturb_word(char *token, int token_len, double budget) {
    // First, Breadth first search to find the nearest 5 words to token in the trie, 
    
    // current_idx is the index in the trie array where we start (i.e. the root)
    int current_idx = 0;
    // current_cnt is the number of nodes at the current level (initially the root level, so 26 for the characters in the alphabet)
    int current_cnt = 26;
    
    // table is the entire trie loaded into memory as an array of Nodes
    Node *table = (Node *)___data_bin;

    int current_depth = 0;

    char *perturb_candidates[10];
    float candidate_costs[10];
    int candidate_distances[10];
    int candidate_count = 0;
    Node *nodes_visited[128];
    int minimum_candidate_cost = 10000;

    // First, BFS to the end of the token in the trie, keeping track of the nodes we visit along the way
    while (current_depth < token_len) {
        Node key = {0};
        key.token = token[current_depth];

        // Binary search for the current character among the current node's children
        Node *match = (Node *)bsearch(&key, &table[current_idx], current_cnt, sizeof(Node), compare_nodes);

        if (match) {
            nodes_visited[current_depth] = match;

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


    // First add original word as a canididate
    if (current_depth == token_len && nodes_visited[current_depth - 1]->is_word) {
        perturb_candidates[candidate_count] = malloc(token_len + 1);
        memcpy(perturb_candidates[candidate_count], token, token_len);
        perturb_candidates[candidate_count][token_len] = '\0';
        
        candidate_costs[candidate_count] = nodes_visited[current_depth - 1]->cost;
        if (candidate_costs[candidate_count] < minimum_candidate_cost) minimum_candidate_cost = candidate_costs[candidate_count];
        candidate_distances[candidate_count] = 0;
        candidate_count++;
    }

    // Find all other perturbation candidates
    for (int depth = current_depth - 1; depth >= 0 && candidate_count < 10; depth--) {
        int level_start_idx = (depth == 0) ? 0 : nodes_visited[depth-1]->child_idx;
        int level_count = (depth == 0) ? 26 : nodes_visited[depth-1]->child_cnt;

        for (int i = 0; i < level_count && candidate_count < 10; i++) {
            Node *sibling = &table[level_start_idx + i];
            
            if (sibling == nodes_visited[depth]) continue;

            // Check if sibling is a word
            if (sibling->is_word) {
                // Construct temporary string to check for duplicates
                char temp[depth + 2];
                for (int k = 0; k < depth; k++) temp[k] = nodes_visited[k]->token;
                temp[depth] = sibling->token;
                temp[depth + 1] = '\0';

                int exists = 0;
                for (int e = 0; e < candidate_count; e++) {
                    if (strcmp(perturb_candidates[e], temp) == 0) {
                        exists = 1; break;
                    }
                }

                if (!exists) {
                    perturb_candidates[candidate_count] = malloc(depth + 2);
                    strcpy(perturb_candidates[candidate_count], temp);
                    candidate_costs[candidate_count] = sibling->cost;
                    if (candidate_costs[candidate_count] < minimum_candidate_cost) minimum_candidate_cost = candidate_costs[candidate_count];
                    candidate_distances[candidate_count] = (token_len - depth) + 1;
                    candidate_count++;
                }
            }

            // Check sibling's children
            if (candidate_count < 10 && sibling->child_idx != -1) {
                for (int j = 0; j < sibling->child_cnt && candidate_count < 10; j++) {
                    Node *child = &table[sibling->child_idx + j];
                    if (child->is_word) {
                        char temp[depth + 3];
                        for (int k = 0; k < depth; k++) temp[k] = nodes_visited[k]->token;
                        temp[depth] = sibling->token;
                        temp[depth + 1] = child->token;
                        temp[depth + 2] = '\0';

                        int exists = 0;
                        for (int e = 0; e < candidate_count; e++) {
                            if (strcmp(perturb_candidates[e], temp) == 0) {
                                exists = 1; break;
                            }
                        }

                        if (!exists) {
                            perturb_candidates[candidate_count] = malloc(depth + 3);
                            strcpy(perturb_candidates[candidate_count], temp);
                            candidate_costs[candidate_count] = child->cost;
                            if (candidate_costs[candidate_count] < minimum_candidate_cost) minimum_candidate_cost = candidate_costs[candidate_count];
                            candidate_distances[candidate_count] = (token_len - depth) + 1;
                            candidate_count++;
                        }
                    }
                }
            }
        }
    }

    // Set the original word's cost as the minimum cost among candidates to ensure it has the highest utility
    candidate_costs[0] = minimum_candidate_cost;

    // Create the utility space for the exponential mechanism (negative cost minus distance)
    double candidate_utilities[10];
    for (int i = 0; i < candidate_count; i++) {
        candidate_utilities[i] = minimum_candidate_cost - candidate_costs[i] - candidate_distances[i];
    }

    // Select a candidate using the exponential mechanism
    int selected_idx = exponential_mechanism(candidate_utilities, candidate_count, budget);

    char* result = strdup(perturb_candidates[selected_idx]);

    for (int i = 0; i < candidate_count; i++) {
        free(perturb_candidates[i]);
    }
    
    return result;
}

/**
 * Perturbs a number token by applying the exponential mechanism
 * to select a new number based on a utility function that is the
 * distance from the original number.
 */
int perturb_number(char *token, int token_len, double budget) {
    int len_num = token_len;
 
    // Find the max number that we can represent with the given number of digits
    int max_num = 1;
    for(int i = 0; i < len_num; i++) max_num *= 10;
    max_num -= 1;

    // Convert the token to an integer
    int token_num = atoi(token);

    // Create utility space
    double *utility_space = malloc((max_num + 1) * sizeof(double));
    for (int i = 0; i <= max_num; i++) {
        utility_space[i] = -1.0 * abs(token_num - i);
    }

    int selection = exponential_mechanism(utility_space, max_num + 1, budget);
    
    free(utility_space);

    return selection;
}

/**
 * Perturbs a special character token by applying the exponential mechanism to 
 * select a new special character based on a utility function that is the distance 
 * from the original character to the new one based on the qwerty keyboard layout.
 */
char perturb_special(char *token, double budget) {
    char *special_chars = "`~!@#$%%^&*()-_=+[{]}\\|;:'\",<.>/?";

    int token_num = -1;
    for (int i = 0; i < strlen(special_chars); i++) {
        if (token[0] == special_chars[i]) {
            token_num = i;
            break;
        }
    }

    double *utility_space = malloc((strlen(special_chars) + 1) * sizeof(double));
    for (int i = 0; i <= strlen(special_chars); i++) {
        utility_space[i] = -1.0 * abs(token_num - i);
    }

    int selection = exponential_mechanism(utility_space, strlen(special_chars) + 1, budget);
    
    free(utility_space);

    if (selection < strlen(special_chars)) {
        return special_chars[selection];
    } else {
        return special_chars[token_num];
    }
}

// Helper to calculate inversions (Swap Distance)
int get_utility(int *indices, int n) {
    int inversions = 0;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (indices[i] > indices[j]) inversions++;
        }
    }
    return -1 * inversions;
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void mcmc_perturb(int *current_indices, int n, float eps) {
    // Number of MCMC steps to perform
    int iterations = 1000;

    int current_u = get_utility(current_indices, n);

    // MCMC Walk
    for (int i = 0; i < iterations; i++) {
        // Propose a neighbor by swapping two random elements
        int idx1 = rand() % n;
        int idx2 = rand() % n;
        if (idx1 == idx2) continue;

        swap(&current_indices[idx1], &current_indices[idx2]);
        int proposal_u = get_utility(current_indices, n);

        // Calculate Acceptance Probability
        // P(accept) = min(1, exp(eps * (prop_u - curr_u) / 2))
        double diff = (double)(proposal_u - current_u);
        double acceptance_prob = exp((eps * diff) / 2.0);

        double r = (double)rand() / (double)RAND_MAX;

        if (r < acceptance_prob) {
            // Accept the move
            current_u = proposal_u;
        } else {
            // Reject: Swap back
            swap(&current_indices[idx1], &current_indices[idx2]);
        }
    }
}

void perturb_semantically(char *pw, int *start_of_token_indicies, double budget) {
    int num_tokens = 0;
    for (int i = 0; i < 20; i++) {
        if (start_of_token_indicies[i] != -1) {
            num_tokens++;
        } else {
            break;
        }
    }

    int original_list[num_tokens];
    for (int i = 0; i < num_tokens; i++) original_list[i] = i;
    

    int total_swaps = 0;

    mcmc_perturb(original_list, num_tokens, budget);

    // create temp password storing original password
    char *temp = strdup(pw);

    // Zero out buffer fpr original password
    for (int i = 0; i < strlen(pw); i++) pw[i] = '\0';

    for (int i = 0; i < num_tokens; i++) {
        int token_idx = original_list[i];
        int next_token_start = (token_idx < num_tokens - 1) ? start_of_token_indicies[token_idx + 1] : strlen(temp);
        int token_len = next_token_start - start_of_token_indicies[token_idx];
        strncat(pw, temp + start_of_token_indicies[token_idx], token_len);
    }
    free(temp);
}