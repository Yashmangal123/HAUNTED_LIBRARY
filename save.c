/* --- paste into save.c (replace old/ellipsed implementations) --- */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "player.h"
#include "save.h"
#include "auth.h"   /* you may still keep user_save_filename for compatibility */

extern Player P;

#define MAX_FILENAME_LEN 64
#define MAX_SLOTS 5   /* keep in sync with main.c */

char *user_slot_filename(int user_id, int slot) {
    if (user_id <= 0 || slot <= 0) return NULL;
    char *s = malloc(MAX_FILENAME_LEN);
    if (!s) return NULL;
    /* produce save_<user_id>_<slot>.dat */
    snprintf(s, MAX_FILENAME_LEN, "save_%d_%d.dat", user_id, slot);
    return s; /* caller must free() */
}

/* Save the Player struct to a specific slot file */
bool save_player_slot(const Player *p, int slot) {
    if (!p || p->user_id <= 0 || slot <= 0) return false;
    char *fn = user_slot_filename(p->user_id, slot);
    if (!fn) return false;
    FILE *f = fopen(fn, "wb");
    if (!f) { free(fn); return false; }
    /* write the Player struct (simple binary save used by project) */
    if (fwrite(p, sizeof(*p), 1, f) != 1) {
        fclose(f); free(fn); return false;
    }
    fclose(f);
    free(fn);
    return true;
}

/* Load a Player from a specific slot file */
bool load_player_slot(Player *p, int user_id, int slot) {
    if (!p || user_id <= 0 || slot <= 0) return false;
    char *fn = user_slot_filename(user_id, slot);
    if (!fn) return false;
    FILE *f = fopen(fn, "rb");
    if (!f) { free(fn); return false; }
    if (fread(p, sizeof(*p), 1, f) != 1) {
        fclose(f); free(fn); return false;
    }
    fclose(f);
    free(fn);
    return true;
}

/* Convenience: keep save_player() but make it use the current player's selected slot */
bool save_player(const Player *p) {
    if (!p || p->user_id <= 0) return false;
    if (p->save_slot > 0) {
        return save_player_slot(p, p->save_slot);
    }
    /* Fallback if no slot selected: write to single-file name if project uses it */
    char *filename = user_save_filename(p->user_id); /* may be defined in auth.c */
    if (!filename) return false;
    FILE *f = fopen(filename, "wb");
    if (!f) { free(filename); return false; }
    bool ok = (fwrite(p, sizeof(*p), 1, f) == 1);
    fclose(f);
    free(filename);
    return ok;
}

/* Similarly, load_player() should by default load the selected slot if present */
bool load_player(Player *p, int user_id) {
    if (!p || user_id <= 0) return false;
    if (p->save_slot > 0) {
        return load_player_slot(p, user_id, p->save_slot);
    }
    /* fallback: try single-file */
    char *filename = user_save_filename(user_id);
    if (!filename) return false;
    FILE *f = fopen(filename, "rb");
    if (!f) { free(filename); return false; }
    bool ok = (fread(p, sizeof(*p), 1, f) == 1);
    fclose(f);
    free(filename);
    return ok;
}

/* ---------- wrappers used by the rest of the project ---------- */
/* Return 0 on success (so existing checks like if(save_game()==0) work) */
int save_game(void) {
    /* require a chosen slot and valid user */
    if (P.user_id <= 0 || P.save_slot <= 0) return -1;
    return save_player_slot(&P, P.save_slot) ? 0 : -1;
}

int load_game(void) {
    if (P.user_id <= 0 || P.save_slot <= 0) return -1;
    return load_player_slot(&P, P.user_id, P.save_slot) ? 0 : -1;
}
