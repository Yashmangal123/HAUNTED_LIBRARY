#ifndef SAVE_H
#define SAVE_H
#include <stdbool.h>
#include "player.h"

int save_game(void);
int load_game(void);
bool save_player(const Player *p);
bool load_player(Player *p, int user_id);
bool save_player_slot(const Player *p,int slot);
bool load_player_slot(Player *p,int user_id,int slot);
char *user_slot_filename(int user_id, int slot); /* returns malloc'd string: caller must free */


#endif
