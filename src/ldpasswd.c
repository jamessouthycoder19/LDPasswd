#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "ldpasswd/perturb.h"
#include "ldpasswd/ldpasswd.h"
#include "ldpasswd/tokenize.h"
#include "ldpasswd/leet.h"

/**
 * Perturbs a password via LDPasswd. Epsilon is the privacy parameter
 * Ensure that the *pw buffer contains twice as much space as the original password to accomodate for growth when perturbing
 */
void perturb_password(char *pw, double eps){
    srand(time(NULL));

    // Array to hold starting indices of each token
    int *start_of_token_indicies = malloc(20 * sizeof(int));
    for (int i = 0; i < 20; i++) {
        start_of_token_indicies[i] = -1;
    }

    // Array to hold starting indices of each token after perturbing
    int *start_of_token_indicies_after_perturbing = malloc(20 * sizeof(int));
    for (int i = 0; i < 20; i++) {
        start_of_token_indicies_after_perturbing[i] = -1;
    }

    int token_count = 0;

    // Array to hold type of each token (word, number, special, or other)
    char *token_types = malloc(20 * sizeof(char));
    for (int i = 0; i < 20; i++) {
        token_types[i] = '\0';
    }

    // Pointer to hold word with non-leetspeaked tokens
    int pw_length = strlen(pw);
    char *unleeted_pw = malloc(pw_length + 1);
    for (int j = 0; j < pw_length + 1; j++) {
        unleeted_pw[j] = '\0';
    }

    // Pointer to hold new password with perturbed tokens
    char *perturbed_pw = malloc(pw_length * 2);
    for (int j = 0; j < pw_length * 2; j++) {
        perturbed_pw[j] = '\0';
    }

    int num_tokens = tokenize_password(pw, start_of_token_indicies, token_types, unleeted_pw);

    // Establish privacy budget for semantic and diction perturbations
    double semantic_budget;
    double diction_budget;
    double budget_per_token;
    if (num_tokens == 1) {
        semantic_budget = 0;
        diction_budget = 0;
        budget_per_token = eps;
    } else {
        semantic_budget = eps / (num_tokens + 1);
        diction_budget = (num_tokens * eps) / (num_tokens + 1);
        budget_per_token = diction_budget / num_tokens;
    }
    
    for (int j = 0; j < 20; j++) {
        if (start_of_token_indicies[j] != -1) {
            start_of_token_indicies_after_perturbing[j] = strlen(perturbed_pw);

            int next_token_start = (j < 19) ? start_of_token_indicies[j + 1] : strlen(pw);
            int token_len;
            if (next_token_start != -1) {
                token_len = next_token_start - start_of_token_indicies[j];
            } else {
                token_len = strlen(pw) - start_of_token_indicies[j];
            }
            if (token_types[j] == 'w') {
                char* perturbed_word = perturb_word(unleeted_pw + start_of_token_indicies[j], token_len, budget_per_token);
                fix_perturbed_password_leet(perturbed_word, pw + start_of_token_indicies[j], pw_length);
                strcat(perturbed_pw, perturbed_word);
                free(perturbed_word);
            } else if (token_types[j] == 'n') {
                int perturbed_number = perturb_number(unleeted_pw + start_of_token_indicies[j], token_len, budget_per_token);
                char num_buf[32];
                sprintf(num_buf, "%0*d", token_len, perturbed_number);
                strcat(perturbed_pw, num_buf);
            } else if (token_types[j] == 's') {
                char perturbed_special = perturb_special(unleeted_pw + start_of_token_indicies[j], budget_per_token);
                perturbed_pw[strlen(perturbed_pw)] = perturbed_special;
            } else {
                strncat(perturbed_pw, unleeted_pw + start_of_token_indicies[j], token_len);
            }
        }
    }

    perturb_semantically(perturbed_pw, start_of_token_indicies_after_perturbing, semantic_budget);

    // Zero out original password buffer and copy perturbed password back in
    for (int i = 0; i < strlen(pw); i++) pw[i] = '\0';
    strncpy(pw, perturbed_pw, strlen(perturbed_pw));

    free(perturbed_pw);
    free(unleeted_pw);
    free(token_types);
    free(start_of_token_indicies);
    free(start_of_token_indicies_after_perturbing);
}