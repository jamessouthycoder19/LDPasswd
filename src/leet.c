#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ldpasswd/leet.h"

// Struct to map leet chars back to letters
typedef struct {
    char leet;
    char original;
} Map;

// Map for Leetspeak substitutions.
Map LEET_MAP[] = {
    {'4', 'a'}, {'@', 'a'}, {'8', 'b'}, {'(', 'c'}, {'{', 'c'}, 
    {'[', 'c'}, {'<', 'c'}, {'3', 'e'}, {'6', 'g'}, {'9', 'g'}, 
    {'1', 'i'}, {'!', 'i'}, {'|', 'i'}, {'1', 'l'}, {'|', 'l'}, 
    {'7', 'l'}, {'0', 'o'}, {'$', 's'}, {'5', 's'}, {'+', 't'}, 
    {'7', 't'}, {'%', 'x'}, {'2', 'z'}
};

/**
 * Function used to add an un-leeted string to the results list, resizing if necessary.
 */
void add_result(ResultList *res, const char *str) {
    if (res->count == res->capacity) {
        res->capacity *= 2;
        res->strings = realloc(res->strings, sizeof(char*) * res->capacity);
    }
    res->strings[res->count++] = strdup(str);
}


/**
 * The following two functions (backtrack and generate_unleet) are utilized 
 * to generate all possible "un-leeted" versions of a given password.
 * 
 * Example: "p@ssw0rd" would generate "password", "p@ssword", "passw0rd", etc.
 * 
 * There are 2 functions because recursion is used :(
 */
void backtrack(const char *input, char *current, int index, ResultList *res) {
    if (input[index] == '\0') {
        current[index] = '\0';
        add_result(res, current);
        return;
    }

    char c = input[index];
    int found = 0;

    // Check the map for all possible translations
    for (int i = 0; i < sizeof(LEET_MAP) / sizeof(Map); i++) {
        if (LEET_MAP[i].leet == c) {
            found = 1;
            current[index] = LEET_MAP[i].original;
            backtrack(input, current, index + 1, res);
        }
    }

    // Also consider the character as itself (e.g., '0' stays '0')
    current[index] = c;
    backtrack(input, current, index + 1, res);
}

/**
 * The following two functions (backtrack and generate_unleet) are utilized 
 * to generate all possible "un-leeted" versions of a given password.
 * 
 * Example: "p@ssw0rd" would generate "password", "p@ssword", "passw0rd", etc.
 * 
 * There are 2 functions because recursion is used :(
 */
ResultList generate_unleet(const char *input) {
    ResultList res;
    res.count = 0;
    res.capacity = 10;
    res.strings = malloc(sizeof(char*) * res.capacity);
    
    char *buffer = malloc(strlen(input) + 1);
    backtrack(input, buffer, 0, &res);
    free(buffer);
    
    return res;
}

void fix_perturbed_password_leet(char *perturbed_pw, char *original_pw, int max_len) {
    for (int i = 0; i < max_len; i++) {
        char c = perturbed_pw[i];

        // first check leeted characters
        for (int j = 0; j < sizeof(LEET_MAP) / sizeof(Map); j++) {
            if (LEET_MAP[j].original == c && LEET_MAP[j].leet == original_pw[i]) {
                perturbed_pw[i] = LEET_MAP[j].original;
                break;
            }
        }

        // Now uppercase/lowercase
        if (toupper(c) == original_pw[i]) {
            perturbed_pw[i] = toupper(c);
        }      
    }
}