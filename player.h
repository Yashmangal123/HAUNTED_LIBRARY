#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>

#define MAX_ITEMS 7
#define ITEM_LEN 28
#define MAX_NAME 32

/* Room identifiers */
enum {
    R_LAB = 1, R_CORRIDOR, R_STORE, R_TEACHER, R_PROJECTOR,
    R_EXIT = 6, R_BASEMENT = 7, R_ARCHIVE = 8
};

/* Player state struct */
typedef struct {
    int user_id;
    char username[MAX_NAME];
    char password[64];
    int save_slot;
    int room;
    int health;
    int has_won;
    char items[MAX_ITEMS][ITEM_LEN];
    int item_count;
    /* puzzle flags */
    int riddle_solved;
    int box_opened;
    int drawer_opened;
    int projector_solved;
    unsigned char switches; /* bits: 2 1 0 => switch1 switch2 switch3 */
} Player;

extern Player P;

/* init */
void init_player(void);

/* inventory */
int has_item(const char *name);
void add_item(const char *name);
void show_inventory(void);

#endif
