#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    printf("Hello, World!\n");
}