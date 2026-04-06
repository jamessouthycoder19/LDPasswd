#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "ldpasswd/perturb.h"
#include "ldpasswd/ldpasswd.h"
#include "ldpasswd/tokenize.h"
#include "ldpasswd/leet.h"

void perturb(char *pw, double eps){
    // Array to hold starting indices of each token
    int *start_of_token_indicies = malloc(20 * sizeof(int));
    for (int i = 0; i < 20; i++) {
        start_of_token_indicies[i] = -1;
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
    double semantic_budget = eps / (num_tokens + 1);
    double diction_budget = (num_tokens * eps) / (num_tokens + 1);
    double budget_per_token = diction_budget / num_tokens;

    for (int j = 0; j < 20; j++) {
        if (start_of_token_indicies[j] != -1) {
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
                strcat(perturbed_pw, &perturbed_special);
            } else {
                strncat(perturbed_pw, unleeted_pw + start_of_token_indicies[j], token_len);
            }
            
        }
    }

    printf("Before: %s\n", pw);
    printf("After : %s\n", perturbed_pw);

    printf("\n");
    free(unleeted_pw);
    free(token_types);
    free(start_of_token_indicies);
}

void hello() {
    srand(time(NULL));
    char test_pws[][64] = {
        "musketeer333marshmallow",
        "!blah#Thing^",
        "Password7JamesTh1ngFather",
        "PassWord123",
        "Unknownword",
        "jamesblahblahblah",
        "P@sSw0Rd",
        "passw0rdRed14&TeamSucks",
        "123PassWord123",
        "1Unknown(((wordBlah",
        "P@sSw0RdJ4mes",
        "Change.me123!",
        "starburst",
        "PleaseDont5489CH32",
        "Super123!?",
        "VikingWorksAgain!",
        "RedTeamIsUgly!!",
        "White#Comet192@Map28",
        "WhyAreWeStillHere2015?",
        "foobarbaz5",
        "weloveyouredteam",
        "RickerBalls2",
        "Idkwhatimdoinghelp1",
        "ThankYouWhiteTeam",
        "Apple432Bottom#",
        "Welcome1",
        "RedTeamSucks3",
        "LemonJumpSlide1#",
        "AngleBatman1!"
    };

    // for (int i = 0; i < 5; i++) {
    for (int i = 0; i < 29; i++) {
        perturb(test_pws[i], 5);
    }
}