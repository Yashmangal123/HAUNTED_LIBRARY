/* include/auth.h */
#ifndef AUTH_H
#define AUTH_H

#include <stdbool.h>

int create_account(const char *username, const char *password); /* returns user_id or -1 on error */
int login_user(const char *username, const char *password);      /* returns user_id or -1 on fail */
bool user_has_save(int user_id);
char *user_save_filename(int user_id); /* returns malloc'd string: caller must free */
int read_minlen_input(const char *prompt, char *buf, size_t bufsz, int min_len);
int delete_account(int user_id);
void trim_inplace(char *s);

#endif 
