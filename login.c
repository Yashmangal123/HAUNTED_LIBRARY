#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "auth.h"
#include "player.h"
#include "save.h"
#include "rooms.h"
#include "puzzles.h"
#include "util.h"
#include "ui.h"
#include "login.h"


/* read_trimmed_line(): prompt, read a line with fgets, strip newline and trim.
 * Returns 1 on success (buf contains trimmed string), 0 on EOF/error.
 * This does NOT enforce minimum length — caller handles that.
 */
static int read_trimmed_line(const char *prompt, char *buf, size_t bufsz) {
    if (!prompt || !buf || bufsz == 0) return 0;
    ui_prompt(prompt);            /* ui_prompt takes a single string argument */
    if (!fgets(buf, (int)bufsz, stdin)) return 0;
    size_t L = strlen(buf);
    if (L > 0 && buf[L-1] == '\n') buf[L-1] = '\0';
    trim_inplace(buf);            /* reuse your existing trimming helper */
    return 1;
}




/* login_menu: handles authentication only then calls slot_menu */
void login_menu(void) {
    while (1) {
        int choice;
        char username[64], password[64];

        ui_boxed_title("LOGIN MENU");
        ui_info("1) Login\n");
        ui_info("2) Create Account\n");
        ui_info("3) Quit\n");
        ui_prompt("Choose: ");

        if (scanf("%d", &choice) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF) {} /* flush */
            ui_warn("Invalid input.\n");
            continue;
        }
        int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} /* flush newline */

        /* ---------------------- LOGIN ---------------------- */
            /* enforce min length */
        if (choice == 1) {
    /* Use the centralized helper that trims and enforces min length */
    if (!read_trimmed_line("Username (or type 'back' to return): ", username, sizeof username)) {
        /* input error (EOF) — just continue */
        continue;
    }

    /* Allow user to return to main login menu */
    if (strcmp(username, "back") == 0) {
        continue;
    }

    /* if not 'back' then enforce min length */
    if ((int)strlen(username) < 5) {
        ui_warn("Username must be at least 5 non-space characters.\n");
        continue;
    }

    /* Read password (allow 'back') */
    if (!read_trimmed_line("Password (or type 'back' to return): ", password, sizeof password)) {
        continue;
    }
    if (strcmp(password, "back") == 0) {
        continue;
    }
    if ((int)strlen(password) < 5) {
        ui_warn("Password must be at least 5 non-space characters.\n");
        continue;
    }

    int uid = login_user(username, password);
    if (uid > 0) {
        /* successful login */
        init_player();              /* sets defaults for P */
        P.user_id = uid;
        strncpy(P.username, username, sizeof P.username - 1);
        P.username[sizeof P.username - 1] = '\0';
        P.save_slot = 0; /* not chosen yet */
        strncpy(P.password, password, sizeof P.password - 1);
        P.password[sizeof P.password - 1] = '\0';

        ui_info("\nLogin successful! Welcome, %s.\n", P.username);

        //ui_clear();

        /* now go into the per-user slot menu */
        slot_menu();

        /* return to login menu after logout */
        continue;
    } else {
        ui_error("Invalid login.\n");
    }
}


        /* ---------------------- CREATE ACCOUNT ---------------------- */
        else if (choice == 2) {
    /* Ask for username with option to return */
    if (!read_trimmed_line("Choose username (or type 'back' to return): ",
                           username, sizeof username)) {
        continue;
    }

    /* back to login menu */
    if (strcmp(username, "back") == 0) {
        continue;
    }

    /* Reject spaces anywhere */
    int bad = 0;
    for (size_t i = 0; username[i] != '\0'; i++) {
        if (isspace((unsigned char)username[i])) {
            bad = 1;
            break;
        }
    }
    if (bad || strlen(username) < 5) {
        ui_warn("Username must be at least 5 characters and contain NO spaces.\n");
        continue;
    }

    /* Ask for password with option to return */
    if (!read_trimmed_line("Choose password (or type 'back' to return): ",
                           password, sizeof password)) {
        continue;
    }

    if (strcmp(password, "back") == 0) {
        continue;  /* back to login menu */
    }

    /* Reject spaces in password */
    bad = 0;
    for (size_t i = 0; password[i] != '\0'; i++) {
        if (isspace((unsigned char)password[i])) {
            bad = 1;
            break;
        }
    }
    if (bad || strlen(password) < 5) {
        ui_warn("Password must be at least 5 characters and contain NO spaces.\n");
        continue;
    }

    int nid = create_account(username, password);
    if (nid > 0) {
          init_player(); 
        P.user_id=nid;
         strncpy(P.username, username, sizeof P.username - 1);
          strncpy(P.password, password, sizeof P.password - 1);
        //ui_clear();
        slot_menu();
        //ui_info("Account created! Your ID is %d.\n", nid);
    }
    else {
        ui_error("Username already taken.\n");
    }
}
        /* ---------------------- QUIT ---------------------- */
        else if (choice == 3) {
            ui_info("Goodbye!\n");
            exit(0);
        }

        /* ---------------------- INVALID ---------------------- */
        else {
            ui_warn("Invalid option.\n");
        }
    }
}