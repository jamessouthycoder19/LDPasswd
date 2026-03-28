#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <json-c/json.h>

#include <ldpasswd/ldpasswd.h>

int tokenize(const char *input) {
    // First get the length of the password
    int length = strlen(input);

    // Array to hold starting indices of each token
    int start_of_token_indicies[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

    int token_count = 0;
    for (int i = 0; i < length; i++) {
        // Only storing 20 tokens
        if (token_count >= 20) {
            break;
        } else {
            start_of_token_indicies[token_count] = i;
            token_count++;
        }
        // First check if we have a number
        if (input[i] >= '0' && input[i] <= '9') {
            for (int j = i + 1; j < length; j++) {
                // figure out if we have a number after
                // If not, break out of the 'j' loop.
                if (input[j] < '0' || input[j] > '9') {
                    break;
                } else {
                    // If we do, increment i again to skip over the number
                    // because that gets counted with the previous token.
                    i = j;
                }
            }
        }
        // Next check if we have a special character.
        // No need to check for any other special characters after,
        // they should each be treated as their own token
        else if ((input[i] >= '!' && input[i] <= '/') || (input[i] >= ':' && input[i] <= '@') || (input[i] >= '[' && input[i] <= '`') || (input[i] >= '{' && input[i] <= '~')) {
            start_of_token_indicies[token_count] = i;
            token_count++;
        }
        // Next letters.
        // If we have a capital.
        //            Then if there is a lowercase letter after, keep going until capital or non letter is found, thats the end of token. There should be at least 2 lowercase
        //                                                   
        // If 
        else if (input[i] >= 'A' && input[i] <= 'Z') {
            // start_of_token_indicies[token_count] = i;
            // token_count++;
            // for (int j = i + 1; j < length; j++) {
            //     // figure out if we have a lowercase letter after
            //     // If not, break out of the 'j' loop.
            //     if (input[j] < 'a' || input[j] > 'z') {
            //         break;
            //     } else {
            //         // If we do, increment i again to skip over the lowercase letter
            //         // because that gets counted with the previous token.
            //         i = j;
            //     }
            // }
        }
    }
    return 0;
}

void hello(void) {
    FILE *file = fopen("/home/james/LDPasswd/data/data.txt", "r");
    if (file == NULL) {
        perror("Unable to open file");
        return 1;
    }

    // Array to hold 6 string pointers
    const char *raw_lines;
    char *temp_line = NULL;
    size_t len = 0;

    // getline handles the memory allocation for the temporary read
    while (getline(&temp_line, &len, file) != -1) {
        
        // Remove trailing newline if present
        temp_line[strcspn(temp_line, "\r\n")] = '\0';

        // Use strdup to create a permanent copy for our array
        raw_lines = strdup(temp_line);
    }

    // Free the temporary buffer used by getline
    free(temp_line);
    fclose(file);

    struct json_tokener *tok = json_tokener_new();
    const struct json_object *json_dict;
    json_dict = json_tokener_parse_ex(tok, raw_lines, strlen(raw_lines));

    json_tokener_free(tok);
    free((void *)raw_lines);

    // At this point all of the JSON wordlists have bene parsed correctly. 
    // This wil obviously get organized better into functions and what not
    // but after this is how we would tokenize a password and what not

    const char *example_password = "assword123!";
    struct json_object *current_level = (struct json_object *)json_dict;
    int p_idx = 0; // Current position in the password string
    int depth = 0;
    int pass_len = strlen(example_password);

    printf("Walking the JSON tree for password: %s\n", example_password);

    while (p_idx < pass_len) {
        struct json_object *next_level = NULL;
        char token2[3] = {0};
        char token1[2] = {0};

        // 1. Try a 2-character match if enough characters remain
        int found_match = 0;
        if (p_idx + 1 < pass_len) {
            token2[0] = example_password[p_idx];
            token2[1] = example_password[p_idx + 1];
            
            if (json_object_object_get_ex(current_level, token2, &next_level)) {
                printf("Match found! 2-char token '%s' at depth %d.\n", token2, depth + 1);
                current_level = next_level;
                p_idx += 2; // Consume 2 chars
                found_match = 1;
            }
        }

        // 2. Fallback to 1-character match if 2-char failed
        if (!found_match) {
            token1[0] = example_password[p_idx];
            if (json_object_object_get_ex(current_level, token1, &next_level)) {
                printf("Match found! 1-char token '%s' at depth %d.\n", token1, depth + 1);
                current_level = next_level;
                p_idx += 1; // Consume 1 char
                found_match = 1;
            }
        }

        if (found_match) {
            depth++;
            // If the new level isn't an object, we've hit a leaf node (end of a word)
            if (!json_object_is_type(current_level, json_type_object)) {
                printf("Reached a leaf node. Search complete.\n");
                break;
            }
        } else {
            printf("No match found for remaining string starting at '%c'.\n", example_password[p_idx]);
            break;
        }
    }

    printf("Deepest successful match reached: %d tokens.\n", depth);


    // Below this is cleanup code to free the memory allocated for the lines and the json objects.
    json_object_put((struct json_object *)json_dict);

    return 0;
}