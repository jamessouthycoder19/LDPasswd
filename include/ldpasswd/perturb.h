#ifndef LDPASSWD_PERTURB_H
#define LDPASSWD_PERTURB_H

char* perturb_word(char *token, int token_len, double budget);
int perturb_number(char *token, int token_len, double budget);
char perturb_special(char *token, double budget);

#endif