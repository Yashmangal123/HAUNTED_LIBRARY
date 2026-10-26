/* auth.c - Authentication system for Haunted Library
 * Uses strtok (portable) instead of strsep.
 * Uses ui_* helpers and read_minlen_input() for robust input.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ui.h"
#include "auth.h"

#define USERS_FILE "users.txt"
#define MAX_LINE   512

/* --------------------------------------------------------------- */
/* Trim leading/trailing whitespace                                */
/* --------------------------------------------------------------- */
void trim_inplace(char *s) {
    if (!s) return;
    char *start = s;

    while (*start && isspace((unsigned char)*start))
        start++;

    if (start != s)
        memmove(s, start, strlen(start) + 1);

    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
        s[--len] = '\0';
}

/* --------------------------------------------------------------- */
/* Simple hash for passwords (NOT cryptographic, demo only)        */
/* --------------------------------------------------------------- */
static unsigned int simple_hash(const char *s) {
    unsigned int h = 5381;
    while (*s)
        h = ((h << 5) + h) + (unsigned char)(*s++); /* h*33 + c */
    return h;
}

/* read_minlen_input:
 *  - prints prompt via ui_prompt()
 *  - reads a line with fgets()
 *  - strips newline, trims whitespace (leading/trailing)
 *  - enforces min_len characters (after trimming)
 *  - rejects if any whitespace character exists inside the trimmed string
 * Returns 1 on valid input, 0 on EOF/error.
 */
int read_minlen_input(const char *prompt, char *buf, size_t bufsz, int min_len) {
    if (!prompt || !buf || bufsz == 0 || min_len < 0) return 0;

    for (;;) {
        ui_prompt(prompt);   /* single-arg prompt as required */
        fflush(stdout);

        if (!fgets(buf, (int)bufsz, stdin)) return 0; /* EOF/error */

        size_t L = strlen(buf);
        if (L > 0 && buf[L-1] == '\n') buf[L-1] = '\0';

        /* Trim leading/trailing whitespace - reuse your trim function */
        trim_inplace(buf);

        /* Enforce minimum length */
        if ((int)strlen(buf) < min_len) {
            ui_warn("Input must be at least %d non-space characters.\n", min_len);
            continue;
        }

        /* Reject if any whitespace character remains inside the string */
        int bad = 0;
        for (size_t i = 0; buf[i] != '\0'; ++i) {
            if (isspace((unsigned char)buf[i])) { bad = 1; break; }
        }
        if (bad) {
            ui_warn("Input may not contain spaces or tabs. Try again.\n");
            continue;
        }

        return 1;
    }
}


/* --------------------------------------------------------------- */
/* LOGIN                                                            */
/* --------------------------------------------------------------- */
int login_user(const char *username, const char *password) {
    if (!username || !password) return 0;

    unsigned int passhash = simple_hash(password);

    FILE *f = fopen(USERS_FILE, "r");
    if (!f) {
        ui_warn("No user database found. Create an account first.\n");
        return 0;
    }

    char line[MAX_LINE];

    while (fgets(line, sizeof(line), f)) {
        /* strip newline */
        size_t L = strlen(line);
        if (L > 0 && line[L - 1] == '\n')
            line[L - 1] = '\0';

        /* tokenize: id : username : hash */
        char *id_str   = strtok(line, ":");
        char *user_str = strtok(NULL, ":");
        char *hash_str = strtok(NULL, ":");

        if (!id_str || !user_str || !hash_str)
            continue;

        if (strcmp(user_str, username) == 0) {
            unsigned int stored_hash = (unsigned int)strtoul(hash_str, NULL, 10);
            fclose(f);

            if (stored_hash == passhash)
                return atoi(id_str);
            else
                return 0;
        }
    }

    fclose(f);
    return 0;
}

/* --------------------------------------------------------------- */
/* CREATE ACCOUNT                                                   */
/* --------------------------------------------------------------- */
int create_account(const char *username, const char *password) {
    if (!username || !password) return 0;

    FILE *f = fopen(USERS_FILE, "a+");
    if (!f) {
        ui_error("Cannot open users file.\n");
        return 0;
    }

    /* check for existing user, find max id */
    rewind(f);
    char line[MAX_LINE];
    unsigned int max_id = 0;

    while (fgets(line, sizeof line, f)) {
        size_t L = strlen(line);
        if (L > 0 && line[L - 1] == '\n')
            line[L - 1] = '\0';

        char *id_str   = strtok(line, ":");
        char *user_str = strtok(NULL, ":");

        if (!id_str || !user_str)
            continue;

        unsigned int id = (unsigned int)atoi(id_str);
        if (id > max_id)
            max_id = id;

        if (strcmp(user_str, username) == 0) {
            fclose(f);
            return 0; /* username already taken */
        }
    }

    unsigned int new_id = max_id + 1;
    unsigned int hash   = simple_hash(password);

    fprintf(f, "%u:%s:%u\n", new_id, username, hash);
    fclose(f);

    ui_info("Account created! Your ID is %u.\n", new_id);
    return (int)new_id;
}

/* --------------------------------------------------------------- */
/* Compatibility helpers                                            */
/* --------------------------------------------------------------- */
bool user_has_save(int user_id) {
    char name[64];
    snprintf(name, sizeof name, "save_%d.dat", user_id);
    FILE *f = fopen(name, "rb");
    if (f) { fclose(f); return true; }
    return false;
}

char *user_save_filename(int user_id) {
    if (user_id <= 0) return NULL;
    char *p = malloc(64);
    if (!p) return NULL;
    snprintf(p, 64, "save_%d.dat", user_id);
    return p;
}


int delete_account(int user_id) {
    FILE *fin = fopen(USERS_FILE, "r");
    if (!fin) return 0;

    FILE *fout = fopen(USERS_FILE ".tmp", "w");
    if (!fout) { fclose(fin); return 0; }

    char line[512];
    int deleted = 0;

    while (fgets(line, sizeof line, fin)) {
        char copy[512];
        strcpy(copy, line);

        char *id_str = strtok(copy, ":");
        if (!id_str) { 
            fputs(line, fout);
            continue;
        }

        int id = atoi(id_str);

        if (id == user_id) {
            deleted = 1;
            continue;   /* skip this line -> deletes user */
        }

        fputs(line, fout);
    }

    fclose(fin);
    fclose(fout);

    if (!deleted) {
        remove(USERS_FILE ".tmp");
        return 0;
    }

    remove(USERS_FILE);
    rename(USERS_FILE ".tmp", USERS_FILE);

    return 1;
}


