#include "player.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>

Player P;

/* inventory helpers */
int has_item(const char *name) {
    for (int i = 0; i < P.item_count; ++i)
        if (strcmp(P.items[i], name) == 0) 
            return 1;
    return 0;
}

void add_item(const char *name) {
    if (P.item_count >= MAX_ITEMS) {
        ui_warn("-> Inventory full! Can't pick up %s.\n\n", name);
        return;
    }

    strncpy(P.items[P.item_count], name, ITEM_LEN - 1);
    P.items[P.item_count][ITEM_LEN - 1] = '\0';
    P.item_count++;

    ui_info("-> You picked up: %s\n\n", name);
}

void show_inventory(void) {
    ui_boxed_title("INVENTORY");

    if (P.item_count == 0) {
        ui_info("You are carrying nothing.\n\n");
        return;
    }

    ui_info("Items (%d/%d):\n", P.item_count, MAX_ITEMS);

    for (int i = 0; i < P.item_count; ++i) {
        ui_print(" - %s\n", P.items[i]);
    }
    ui_print("\n");
}

void init_player(void) {
    P.room = R_LAB;
    P.health = 100;
    P.has_won = 0;
    P.item_count = 0;

    P.riddle_solved = 0;
    P.box_opened = 0;
    P.drawer_opened = 0;
    P.projector_solved = 0;
    P.switches = 0;

    for (int i = 0; i < MAX_ITEMS; ++i)
        P.items[i][0] = '\0';
}
