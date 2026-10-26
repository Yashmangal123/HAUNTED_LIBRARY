#include "puzzles.h"
#include "player.h"
#include "util.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int codecheck;

/* Corridor riddle (answer: shadow) */
void puzzle_riddle(void) {
    if (P.riddle_solved) { 
        ui_info("You already solved it.\n\n"); 
        return; 
    }

    char buf[128];

    ui_boxed_title("Corridor Riddle");
    ui_info("\"I have many windows but no glass to clean,Inside me, worlds you've never seen.\"\n");
    ui_prompt("Your answer: ");

    if (!fgets(buf, sizeof(buf), stdin)) return;
    strip_nl(buf); to_lower(buf);

    if (strstr(buf, "computer")||strstr(buf,"laptop")) {
        ui_info("Correct. You find a scribbled clue: 2415.\n");
        P.riddle_solved = 1;
        add_item("scribble 2415");
    } else {
        ui_warn("Wrong. The corridor chills you (-10 health).\n");
        P.health -= 10; 
        if (P.health < 0) P.health = 0;
        ui_warn("Health: %d\n\n", P.health);
    }
}

/* Store box ROT13 puzzle: text 'URYyb PBQR' -> 'Hello Code' */
void puzzle_box(void) {
    if (P.box_opened) { 
        ui_info("Box already opened.\n\n"); 
        return; 
    }

    char buf[128];

    ui_boxed_title("Locked Box Puzzle");
    ui_info("Box inscription: 'URYyb PBQR' (it hints ROT13).\n");
    ui_prompt("Decode and type the plaintext: ");

    if (!fgets(buf, sizeof(buf), stdin)) return;
    strip_nl(buf); to_lower(buf);

    /* expected 'hello code' */
    if (strstr(buf, "hello code") || strstr(buf, "hellocode")) {
        ui_info("Correct. The box yields a SILVER KEY and a note 'archive'.\n");
        P.box_opened = 1;
        add_item("silver key");
        add_item("note archive");
    } else {
        ui_warn("Incorrect. The box sparks (-5 health).\n");
        P.health -= 5; 
        if (P.health < 0) P.health = 0;
        ui_warn("Health: %d\n\n", P.health);
    }
}

/* Drawer logic puzzle: expected answer "b c e" (primes: 2,3,5 => map b,c,e) */
void puzzle_drawer(void) {
    if (P.drawer_opened) { 
        ui_info("Drawer already open.\n\n"); 
        return; 
    }

    char buf[128];

    ui_boxed_title("Drawer Logic Puzzle");
    ui_info("Note: 'Order the buttons by the ascending order of the first three primes under 10.'\n");
    ui_prompt("Enter sequence (e.g. 'A B C'): ");

    if (!fgets(buf, sizeof(buf), stdin)) return;
    strip_nl(buf); to_lower(buf);

    if (strstr(buf, "b c e") || strstr(buf, "bce")) {
        ui_info("Drawer clicks open — you found your STUDENT ID.\n");
        P.drawer_opened = 1;
        add_item("student id");
    } else {
        ui_warn("Wrong sequence. A small shock (-10 health).\n");
        P.health -= 10; 
        if (P.health < 0) P.health = 0;
        ui_warn("Health: %d\n\n", P.health);
    }
}

/* Projector math puzzle: missing number in squares -> 36 */
void puzzle_projector(void) {
    if (P.projector_solved) { 
        ui_info("Already solved.\n\n"); 
        return; 
    }

    char buf[64]; 
    int ans;

    ui_boxed_title("Projector Puzzle");
    ui_info("Find the missing number: 33, 4, 74, 45, 16, 86, ?\n");
    ui_prompt("Your answer: ");

    if (!fgets(buf, sizeof(buf), stdin)) return;
    strip_nl(buf);

    if (!parse_int(buf, &ans)) { 
        ui_warn("Not a number. (-5 health)\n"); 
        P.health -= 5; 
        if (P.health < 0) P.health = 0; 
        ui_warn("Health: %d\n\n", P.health); 
        return; 
    }

    if (ans == 57) {
        ui_info("Correct — a hidden stair unlocks to the basement.\n");
        P.projector_solved = 1;
    } else {
        ui_warn("Wrong. The projector screeches (-10 health).\n");
        P.health -= 10; 
        if (P.health < 0) P.health = 0;
        ui_warn("Health: %d\n\n", P.health);
    }
}

/* Basement switch/code puzzle */
void flip_switch(int n) {
    if (n < 1 || n > 3) { 
        ui_warn("Switch must be 1..3\n\n"); 
        return; 
    }

    int bit = 1 << (3 - n); /* map: flip 1 -> bit 2 (0b100), flip 2 -> bit1, flip3 -> bit0 */
    P.switches ^= (unsigned char)bit;

    ui_info("Flipped %d. Switches now: [%d %d %d]\n\n",
           n,
           (P.switches & 0b100) ? 1 : 0,
           (P.switches & 0b010) ? 1 : 0,
           (P.switches & 0b001) ? 1 : 0);
}

/* We implement enter_code as public below properly. */
void enter_code(const char *digits) {
    int code = atoi(digits);
    codecheck=code;

    if (P.switches == 0b101 && code == 241) {
        ui_info("Keypad accepts code. A door opens to the Archive.\n");
        P.room = R_ARCHIVE;
    } else {
        ui_warn("Incorrect code/wrong switch pattern. Shock! (-15 health)\n");
        P.health -= 15; 
        if (P.health < 0) P.health = 0;
        ui_warn("Health: %d\n\n", P.health);
    }
}

/* Archive chest: only opens if switches correct and player uses the silver key or 'take chest' attempt triggers it */
void try_archive_chest(void) {
    if (P.switches == 0b101 && has_item("silver key")) {
        ui_info("Using SILVER KEY with the correct switch pattern opens the chest. You find a CLOCKWORK.\n");
        add_item("clockwork");
    } else {
        ui_warn("Chest locked. Maybe keys, switches or codes are needed.\n\n");
    }
}
