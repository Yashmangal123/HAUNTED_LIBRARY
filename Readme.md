# Haunted Library — C Project  
### Text-Based Adventure Game with User Accounts & Multiple Save Slots  
---

## 1. Overview
Haunted Library is a text-based adventure game written in C.  
It includes room navigation, puzzles, an inventory system, and a full user account system with multiple save slots.  
The project demonstrates file handling, modular C design, structs, string processing, and interactive gameplay.

This project was developed for academic submission as part of the course **[insert course name]**.

---

## 2. Features

### 🔐 User Account System
- Create account (username + password required, minimum 5 chars)
- Login using valid credentials
- **No spaces allowed** in username or password
- Password-supported **Delete Account** feature
- Accounts stored in `users.txt`
- Each user has **five save slots**

### 💾 Save & Load System
- Autosave inside in-game menu
- Manual save and load options
- Save files stored per user and per slot
- Each save includes **the entire Player struct**

### 🧭 Game Mechanics
- Move between rooms: `go north/south/east/west`
- Inspect objects, read riddles, and pick up items
- Several puzzles integrated into the environment
- Health system with random spooky events
- Win by solving all required puzzles and opening the Exit Door

### 🎒 Inventory System
- Holds up to 10 items
- Items persist through saves
- Includes: flashlight, map, keys, notes, clockwork piece, etc.

### 🏛 Rooms Included
- Lab  
- Corridor  
- Store Room  
- Teacher's Table  
- Projector Room  
- Basement  
- Archive  
- Exit Door  

Each room has unique hints, interactions, and puzzle connections.

---

## 3. File Structure

```
HauntedLibrary/
│
├── main.c
├── login.c
├── auth.c
├── player.c
├── rooms.c
├── puzzles.c
├── save.c
├── util.c
├── ui.c
│
├── auth.h
├── login.h
├── player.h
├── rooms.h
├── puzzles.h
├── save.h
├── util.h
├── ui.h
│
├── Makefile
├── users.txt                  (auto-generated)
├── save_user_*_slot_*.dat     (auto-generated)
│
├── README.md
└── team.txt
```

---

## 4. How to Compile

### 🟦 On Linux / macOS
```sh
make
```

### 🟧 On Windows (with MSYS2 MinGW64)
1. Open **MSYS2 MinGW 64-bit** terminal  
2. Navigate to the project folder:
   ```sh
   cd /c/Users/<YourName>/Desktop/HauntedLibrary
   ```
3. Run:
   ```sh
   make
   ```

Executable produced:
- Linux/macOS → `./haunted`
- Windows → `haunted.exe`

---

## 5. How to Run

### Linux/macOS:
```sh
./haunted
```

### Windows (MSYS2):
```sh
./haunted.exe
```

---

## 6. In-Game Commands

### Movement:
```
go north
go south
go east
go west
```

### Interaction:
```
take <item>
inspect box / inspect drawer
read riddle
solve projector
flip 1 / flip 2 / flip 3
enter code ####
use student id
look
inventory or i
help
```

### Saving & menu:
```
save
load
menu
quit
```

### In-game Menu:
```
1) Resume
2) Save now
3) Return to Slot Menu
4) Quit game
```

---

## 7. Save File Information

Each save slot stores the entire game state, including:

- Current room  
- Player health  
- Inventory items  
- Puzzle states  
- Switch configuration  
- Win status  
- Username + user_id  
- Save slot number  

Filename format:
```
save_user_<id>_slot_<slot>.dat
```

---

## 8. users.txt Format

Stored as:

```
<id>:<username>:<hash>
```

Example:
```
1:yash34:284833223
3:john12:94822910
```

---

## 9. Known Limitations
- Password characters visible when typed (no masking)
- Text-only interface
- No colored terminal output on Windows without ANSI support
- Spaces not allowed in username or password

---

## 10. Deleting an Account

1. Login to the account  
2. Choose option **4) Delete Account** in the slot menu  
3. Re-enter your password to confirm  
4. All save slots for that account are deleted  
5. The user entry is removed from `users.txt`  

---

## 11. Team Members  
See `team.txt` for contributors.

---

## 12. License  
This project is intended for academic and educational use only.

