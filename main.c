#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "auth.h"
#include "player.h"
#include "rooms.h"
#include "puzzles.h"
#include "save.h"
#include "util.h"
#include "ui.h"
#include "login.h"
#define MAX_SLOTS 5



/*       HELP MENU (uses ui)                 */
static void show_help(void) {
    ui_boxed_title("Commands (type without quotes)");
    ui_info("  menu           - open in-game menu (autosave, save, load, quit)\n");
    ui_info("  go north       - move north (also south/east/west)\n");
    ui_info("  take <item>    - pick up items\n");
    ui_info("  inspect <thing> / read riddle / solve projector\n");
    ui_info("  flip 1|2|3     - toggle basement switches\n");
    ui_info("  enter code #### - enter keypad code\n");
    ui_info("  take chest     (in archive)\n");
    ui_info("  inventory (or i), look, save, load, help, quit\n\n");
}

/* in_game_menu: show a menu while playing; autosaves on entry (uses ui) */
static int in_game_menu(void) {
    /* Returns:
       0 = resume game
       1 = save now and continue
       2 = go to slot menu (start new / load another slot) -> caller should return to login_menu
       3 = quit program
    */
    char line[64];
    int choice = 0;

    ui_boxed_title("IN-GAME MENU");
    /* Autosave when opening the menu */
    ui_info("Autosaving your progress...\n");
    if (save_game() == 0) {
        ui_info("Autosave successful (slot %d).\n", P.save_slot);
    } else {
        ui_warn("Autosave failed. Make sure a save slot is selected.\n");
    }

    for (;;) {
        ui_info("\nChoose:\n");
        ui_info(" 1) Resume game\n");
        ui_info(" 2) Save now\n");
        ui_info(" 3) Return to Slot Menu (choose new slot / load other slot)\n");
        ui_info(" 4) Quit to desktop\n");
        ui_prompt("Selection: ");

        if (!fgets(line, sizeof line, stdin)) return 0;
        strip_nl(line);

        if (sscanf(line, "%d", &choice) != 1) {
            ui_warn("Invalid input.\n");
            continue;
        }

        if (choice == 1) {
            return 0; /* resume */
        } else if (choice == 2) {
            if (save_game() == 0) ui_info("Saved successfully.\n");
            else ui_warn("Save failed.\n");
            /* remain in menu so player can choose next action */
        } else if (choice == 3) {
            /* go back to slot menu; caller should handle returning to login/slot flow */
            return 2;
        } else if (choice == 4) {
            return 3; /* quit */
        } else {
            ui_warn("Choose 1-4.\n");
        }
    }
}

/*            MAIN GAME LOOP STARTING          */
void run_game_loop(void){
    char line[160], cmd[32], arg[128];

    srand((unsigned)time(NULL));
    ui_info("Starting new game.\n\n");
    ui_print_banner("HAUNTED LIBRARY");

    /* Attempt to load only if a valid user and slot are set by the caller.
       load_game() returns 0 on success. */
    if (P.user_id > 0 && P.save_slot > 0) {
        if (load_game() == 0) {
            ui_info("Loaded saved game (slot %d).\n\n", P.save_slot);
        } else {
            ui_warn("No save found in slot %d — starting a NEW game in that slot.\n\n", P.save_slot);
            init_player();
        }
    } else {
        /* No pre-selected slot -> start a fresh player state */
        init_player();
        //ui_clear();
    }
    show_help();

    while (P.health > 0 && !P.has_won) {
        ui_boxed_title("STATUS");
        ui_info("Health: %d | Room: %s\n", P.health, room_name(P.room));
        ui_print("------------------------------------------------\n");
        describe_room();

        /* Auto: if at exit, check win conditions */
        if (P.room == R_EXIT) {
            /* require student id, box solved, and switches pattern matched (0b101) */
            if (has_item("student id") && P.box_opened && P.switches == 0b101) {
                ui_info("You swipe student id and the conditions met -> Door opens. YOU WIN!\n");
                P.has_won = 1;
                save_game();
                break;
            } else {
                ui_warn("Door won't open — you are pushed back to the projector room.\n\n");
                P.room = R_PROJECTOR;
                continue;
            }
        }

        ui_prompt("> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        strip_nl(line);
        to_lower(line);

        /* parse command and arg */
        cmd[0] = arg[0] = '\0';
        if (sscanf(line, "%31s %127[^\n]", cmd, arg) < 1) { continue; }

        if (strcmp(cmd, "quit") == 0) {
            ui_info("Quitting. Autosave done.\n");
            save_game();
            break;
        } else if (strcmp(cmd, "menu") == 0 || strcmp(cmd, "m") == 0) {
            int menu_result = in_game_menu();
            if (menu_result == 0) {
                /* resume game */
                continue;
            } else if (menu_result == 1) {
                /* saved, resume */
                continue;
            } else if (menu_result == 2) {
                /* go back to slot menu: return to caller which will go back to login/slot selection */
                ui_info("Returning to slot menu...\n");
                return; /* caller (login flow) is responsible for showing slot menu / login_menu */
            } else if (menu_result == 3) {
                /* quit program */
                ui_info("Quitting game. Goodbye.\n");
                exit(0);
            }
        }
        else if (strcmp(cmd, "help") == 0) {
            show_help();
        } else if (strcmp(cmd, "save") == 0) {
            if (save_game() == 0) ui_info("Saved.\n\n"); else ui_warn("Save failed.\n\n");
        } else if (strcmp(cmd, "load") == 0) {
            if (load_game() == 0) ui_info("Loaded.\n\n"); else ui_warn("No save found.\n\n");
        } else if (strcmp(cmd, "inventory") == 0 || strcmp(cmd, "i") == 0) {
            show_inventory();
        } else if (strcmp(cmd, "look") == 0) {
            /* describe at top of loop will run again */
        }
        /* Movement */
        else if (strcmp(cmd, "go") == 0) {
            if (strstr(arg, "north")) move_north();
            else if (strstr(arg, "south")) move_south();
            else if (strstr(arg, "east")) move_east();
            else if (strstr(arg, "west")) move_west();
            else ui_info("Use north/south/east/west\n\n");
        }
        /* Take items (simple cases) */
        else if (strcmp(cmd, "take") == 0) {
            if (P.room == R_LAB && strstr(arg, "flashlight") && !has_item("flashlight")) add_item("flashlight");
            else if (P.room == R_LAB && strstr(arg, "map") && !has_item("map")) add_item("map");
            else if (P.room == R_STORE && (strstr(arg, "key") || strstr(arg, "rusty")) && !has_item("rusty key")) add_item("rusty key");
            else if (P.room == R_ARCHIVE && strstr(arg, "chest")) try_archive_chest();
            else ui_info("Nothing to take or you already have it.\n\n");
        }
        /* Puzzle triggers */
        else if ((strcmp(cmd, "read") == 0 && strstr(arg, "riddle")) || (strcmp(cmd, "riddle") == 0)) {
            puzzle_riddle();
        } else if ((strcmp(cmd, "inspect") == 0 && strstr(arg, "box")) || (strcmp(cmd, "inspect") == 0 && strstr(arg, "store"))) {
            puzzle_box();
        } else if ((strcmp(cmd, "inspect") == 0 && strstr(arg, "drawer")) || (strcmp(cmd, "drawer") == 0)) {
            puzzle_drawer();
        } else if ((strcmp(cmd, "solve") == 0 && strstr(arg, "projector")) || (strcmp(cmd, "projector") == 0)) {
            puzzle_projector();
        } else if (strcmp(cmd, "flip") == 0) {
            if (P.room != R_BASEMENT) { ui_info("No switches here.\n\n"); }
            else {
                int n = atoi(arg);
                flip_switch(n);
            }
        } else if (strcmp(cmd, "enter") == 0 && strstr(arg, "code")) {
            if (P.room != R_BASEMENT) { ui_info("No keypad here.\n\n"); }
            else {
                /* extract digits from arg */
                char *p = arg;
                while (*p && !isdigit((unsigned char)*p)) ++p;
                if (!*p) { ui_warn("Specify digits after 'enter code'\n\n"); }
                else enter_code(p);
            }
        } else if (strcmp(cmd, "use") == 0 && strstr(arg, "student")) {
            if (has_item("student id")) ui_info("You have the STUDENT ID. Try the exit when ready.\n\n");
            else ui_info("You don't have a student id.\n\n");
        } else {
            ui_warn("Unknown command. Type 'help'.\n\n");
        }

        /* random spooky event to keep risk */
        if ((rand() % 12) == 0 && P.health > 0 && !P.has_won) {
            ui_warn("*** The lights flicker — you feel uneasy. (-5 health) ***\n\n");
            P.health -= 5; if (P.health < 0) P.health = 0;
        }
    }

    if (P.health <= 0) ui_error("You were caught by the ghost. GAME OVER.\n");
    if (P.has_won) ui_info("Congratulations — you escaped!\n");
    ui_print("Thanks for playing!\n");
    return;
}

/*                                 Slot menu (uses ui)                                            */
/* slot_menu: choose slot, with safety when slot already used (Option B) */
void slot_menu(void) {
    int choice;
    while (1) {
        ui_boxed_title("MAIN MENU");
        ui_info("Player: %s (id=%d)\n", P.username, P.user_id);
        ui_info("1) Start NEW game (choose slot 1-%d)\n", MAX_SLOTS);
        ui_info("2) Load existing saved slot\n");
        ui_info("3) Logout (back to Login menu)\n");
        ui_info("4) Delete Account\n");
        ui_prompt("Choose: ");

        if (scanf("%d", &choice) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            ui_warn("Invalid input.\n");
            continue;
        }
        int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}

        /* ======================= OPTION 1: NEW GAME ======================= */
        if (choice == 1) {

            ui_info("Choose slot to save into (1-%d):\n", MAX_SLOTS);
            ui_prompt("> ");

            int slot;
            if (scanf("%d", &slot) != 1) {
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                ui_warn("Bad input.\n");
                continue;
            }
            while ((ch = getchar()) != '\n' && ch != EOF) {}

            if (slot < 1 || slot > MAX_SLOTS) {
                ui_warn("Invalid slot.\n");
                continue;
            }

            /* check if save exists */
            char *fn = user_slot_filename(P.user_id, slot);
            int exists = 0;
            if (fn) {
                FILE *f = fopen(fn, "rb");
                if (f) { exists = 1; fclose(f); }
            }

            /* Slot has an existing save */
            if (exists) {
                ui_boxed_title("SLOT OCCUPIED");
                ui_info("Slot %d already contains a saved game.\n", slot);
                ui_info("1) Continue saved game\n");
                ui_info("2) Erase and start NEW game\n");
                ui_info("3) Cancel\n");
                ui_prompt("Choose: ");

                int sub = 0;
                if (scanf("%d", &sub) != 1) {
                    while ((ch = getchar()) != '\n' && ch != EOF) {}
                    ui_warn("Invalid input.\n");
                    if (fn) free(fn);
                    continue;
                }
                while ((ch = getchar()) != '\n' && ch != EOF) {}

                /* ---- SUB OPTION 1: Continue saved game ---- */
                if (sub == 1) {
                    P.save_slot = slot;
                    if (load_game() == 0)
                        ui_info("Loaded slot %d. Resuming game.\n", slot);
                    else {
                        ui_warn("Failed to load slot %d. Starting new.\n", slot);
                        init_player();
                        P.save_slot = slot;
                    }
                    if (fn) free(fn);
                    run_game_loop();
                    continue;
                }

                /* ---- SUB OPTION 2: Erase and start NEW ---- */
                else if (sub == 2) {
                    P.save_slot = slot;
                    init_player();
                    if (fn) remove(fn);
                    ui_info("Starting NEW game in slot %d.\n", slot);
                    if (fn) free(fn);
                    run_game_loop();
                    continue;
                }

                /* ---- SUB OPTION 3: Cancel ---- */
                else {
                    if (fn) free(fn);
                    ui_info("Cancelled.\n");
                    continue;
                }
            }

            /* Slot empty — start new game */
            if (fn) free(fn);
            P.save_slot = slot;
            init_player();
            ui_info("Starting NEW game in slot %d.\n", slot);
            run_game_loop();
            continue;
        }

        /* ======================= OPTION 2: LOAD GAME ======================= */
        else if (choice == 2) {
            ui_info("Available saved slots:\n");
            int found = 0;

            for (int s = 1; s <= MAX_SLOTS; s++) {
                char *fn = user_slot_filename(P.user_id, s);
                if (!fn) continue;
                FILE *f = fopen(fn, "rb");
                if (f) {
                    fclose(f);
                    ui_info("  Slot %d: %s\n", s, fn);
                    found = 1;
                }
                free(fn);
            }

            if (!found)
                ui_warn("(none)\n");

            ui_info("Enter slot to load 1-%d (0 to cancel):\n", MAX_SLOTS);
            ui_prompt("> ");

            int slot;
            if (scanf("%d", &slot) != 1) {
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                ui_warn("Bad input.\n");
                continue;
            }
            while ((ch = getchar()) != '\n' && ch != EOF) {}

            if (slot == 0) continue;
            if (slot < 1 || slot > MAX_SLOTS) {
                ui_warn("Invalid slot.\n");
                continue;
            }

            P.save_slot = slot;
            if (load_game() == 0)
                ui_info("Loaded slot %d. Resuming game.\n", slot);
            else {
                ui_warn("Failed to load slot %d. Starting NEW.\n", slot);
                init_player();
            }

           // ui_clear();

            run_game_loop();
            continue;
        }

        /* ======================= OPTION 3: LOGOUT ======================= */
        else if (choice == 3) {
            ui_info("Logging out...\n");
            //ui_clear();
            P.user_id = 0;
            P.save_slot = 0;
            return;
        }


        /* ======================= OPTION 4 : Delete account ======================= */
        else if (choice == 4) {
    char pass[64];

    ui_info("You are about to DELETE your account permanently.\n");
    ui_info("This will remove all save files.\n");

    if (!read_minlen_input("Enter password to confirm: ", pass, sizeof(pass), 5)) {
        ui_warn("Invalid password entry.\n");
        //continue;
    }

    if (strcmp(pass, P.password) != 0) {
    ui_warn("Password incorrect. Account NOT deleted.\n");
    continue;
}

/* proceed to call delete_account_by_id_or_call_auth() which removes from file */
if (!delete_account(P.user_id)) {
    ui_warn("Failed to delete account (file error).\n");
    continue;
}
    ui_info("Account deleted successfully.\n");
    //ui_clear();
    /* remove all slot saves */
    for (int s = 1; s <= MAX_SLOTS; s++) {
        char *fn = user_slot_filename(P.user_id, s);
        if (fn) { remove(fn); free(fn); }
    }

    /* log out */
    P.user_id = 0;
    P.save_slot = 0;
    P.username[0] = '\0';

    return;  /* go back to login menu */
}



        else {
            ui_warn("Invalid option.\n");
        }
    }
}



int main(void) {
    ui_init();
    login_menu();
    return 0;
}
