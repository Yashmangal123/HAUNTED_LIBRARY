#include "rooms.h"
#include "player.h"
#include "ui.h"
#include <stdio.h>

/* Room names & description */
const char *room_name(int r) {
    switch (r) {
        case R_LAB: return "Dark Computer Lab";
        case R_CORRIDOR: return "Creaky Corridor";
        case R_STORE: return "Store Room";
        case R_TEACHER: return "Teacher's Table";
        case R_PROJECTOR: return "Old Projector Room";
        case R_BASEMENT: return "Basement Switch Room";
        case R_ARCHIVE: return "Archive Room";
        case R_EXIT: return "Exit Door";
        default: return "Unknown";
    }
}

/* Describe current room and short hints */
void describe_room(void) {
    ui_boxed_title(room_name(P.room));
    switch (P.room) {
    case R_LAB:
        ui_info("You are in the Dark Computer Lab. Monitors flicker.\n");
        if(!has_item("map") || !has_item("flashlight")){ui_info("On the floor:\t");
        if (!has_item("flashlight")) ui_info("Flashlight ");
        if (!has_item("map")) ui_info("MAP");}
        printf("\n");
        if (!has_item("flashlight")) ui_info("Type 'take flashlight' to pick it up.\n");
        if (!has_item("map")) ui_info("Type 'take map' to pick it up.\n");
        ui_info("Exits: north (Corridor)\n");
        break;

    case R_CORRIDOR:
        ui_info("Creaky Corridor.\n");
        if(!P.riddle_solved){ui_info(" A plaque contains a riddle.\n");}
        if (!P.riddle_solved) ui_info("Type 'read riddle' to read the plaque.\n");
        ui_info("Exits: south (Lab), east (Store), west (Teacher), north (Projector)\n");
        break;

    case R_STORE:
        ui_info("Dusty Store Room.\n");
        if (!P.box_opened) ui_info(" There's a locked box with a short cipher inscription.\n");
        if (!P.box_opened) ui_info("Type 'inspect box' to inspect it.\n");
        if (!has_item("rusty key")){
            if (!P.box_opened){ ui_info("You notice glimmering metal under a crate.\n");};
        }
        ui_info("Exits: west (Corridor)\n");
        break;

    case R_TEACHER:
        ui_info("Teacher's Table.\n");
        if (!P.drawer_opened) ui_info(" A locked drawer with three buttons sits here.\n");
        if (!P.drawer_opened) ui_info("Type 'inspect drawer' to try the logic puzzle.\n");
        ui_info("Exits: east (Corridor)\n");
        break;

    case R_PROJECTOR:
        ui_info("Old Projector Room.\n");
        if (!P.projector_solved) ui_info(" A math sequence is on the wall.Type 'solve projector' to attempt solving the puzzle.\n");
        else ui_info("A hidden stair has been revealed leading to the basement.\n");
        if (P.projector_solved)
            ui_info("Exits: south (Corridor), north (Basement)\n");
        else
            ui_info("Exits: south (Corridor)\n");
        break;

    case R_BASEMENT:
        ui_info("Basement: three switches labeled 1,2,3 and a keypad 'CODE: ____'.\n");
        ui_info("Commands: 'flip 1' 'flip 2' 'flip 3' and 'enter code ###'.\n");
        ui_info("Exits: south (Projector), north (Archive)\n");
        break;

    case R_ARCHIVE:
        ui_info("Archive Room: old journals and a locked chest.\n");
        if (P.switches == 0b101) ui_info("The chest seems to respond to a special pattern.\n");
        ui_info("Exits: south (Basement), north (Exit Door)\n");
        break;

    case R_EXIT:
        ui_info("The EXIT DOOR stands before you.\n");
        ui_info("Exits: south (Archive)\n");
        break;

    default:
        ui_info("A quiet corridor.\n");
        ui_info("Exits: none\n");
        break;
    }
    ui_print("\n");
}

/* Movement functions */
void move_north(void) {
    extern int codecheck;
    if (P.room == R_LAB) P.room = R_CORRIDOR;
    else if (P.room == R_CORRIDOR) P.room = R_PROJECTOR;
    else if (P.room == R_PROJECTOR) {
        if (P.projector_solved) P.room = R_BASEMENT;
        else { ui_warn("You don't see a stair up here. Solve the projector puzzle.\n\n"); }
    }
    else if (P.room == R_BASEMENT){if (P.switches == 0b101 && codecheck == 241){P.room=R_ARCHIVE;}
else{ui_warn("Solve switch puzzle to proceed!\n");}}
    else ui_warn("Can't go north from here.\n\n");
}
void move_south(void) {
    if (P.room == R_CORRIDOR) P.room = R_LAB;
    else if (P.room == R_PROJECTOR) P.room = R_CORRIDOR;
    else if (P.room == R_BASEMENT) P.room = R_PROJECTOR;
    else if (P.room == R_ARCHIVE) P.room = R_BASEMENT;
    else ui_warn("Can't go south from here.\n\n");
}
void move_east(void) {
    if (P.room == R_CORRIDOR) P.room = R_STORE;
    else if (P.room == R_TEACHER) P.room = R_CORRIDOR;
    else if (P.room == R_ARCHIVE) P.room = R_EXIT;
    else ui_warn("Can't go east from here.\n\n");
}
void move_west(void) {
    if (P.room == R_CORRIDOR) P.room = R_TEACHER;
    else if (P.room == R_STORE) P.room = R_CORRIDOR;
    else ui_warn("Can't go west from here.\n\n");
}
