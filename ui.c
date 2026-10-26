#include "ui.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif


/* ANSI color codes */
#define ANSI_RESET    "\x1b[0m"
#define ANSI_BOLD     "\x1b[1m"
#define ANSI_RED      "\x1b[31m"
#define ANSI_GREEN    "\x1b[32m"
#define ANSI_YELLOW   "\x1b[33m"
#define ANSI_BLUE     "\x1b[34m"
#define ANSI_MAGENTA  "\x1b[35m"
#define ANSI_CYAN     "\x1b[36m"
#define ANSI_WHITE    "\x1b[37m"


void ui_init(void) {
#ifdef _WIN32
    /* Enable ANSI processing on Windows 10+ */
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(hOut, &mode)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, mode);
        }
    }
#endif
}

// void ui_clear(void) {
//     /* try both ANSI and fallback */
//     printf("\x1b[2J\x1b[H");
//     fflush(stdout);
// }

static void vprint_color(const char *color, const char *fmt, va_list ap) {
    if (color) fputs(color, stdout);
    vprintf(fmt, ap);
    if (color) fputs(ANSI_RESET, stdout);
    fflush(stdout);
}

void ui_print(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprint_color(NULL, fmt, ap);
    va_end(ap);
}

void ui_info(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    vprint_color(ANSI_CYAN, fmt, ap);
    va_end(ap);
}

void ui_warn(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    vprint_color(ANSI_YELLOW, fmt, ap);
    va_end(ap);
}

void ui_error(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    vprint_color(ANSI_RED ANSI_BOLD, fmt, ap);
    va_end(ap);
}

void ui_prompt(const char *prompt) {
    printf(ANSI_GREEN "%s" ANSI_RESET, prompt);
    fflush(stdout);
}

void ui_wait_for_enter(void) {
    ui_print("\nPress Enter to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int ui_term_width(void) {
    /* best-effort: try environment or fallback to 80 */
#ifdef _WIN32
    /* not implementing complex API for now */
    (void)0;
#endif
    char *cols = getenv("COLUMNS");
    if (cols) return atoi(cols) > 20 ? atoi(cols) : 80;
    return 80;
}

void ui_boxed_title(const char *title) {
    int w = ui_term_width();
    int len = (int)strlen(title);
    int pad = (w - len - 2) / 2;
    if (pad < 0) pad = 0;
    for (int i = 0; i < w; ++i) putchar('=');
    putchar('\n');
    for (int i = 0; i < pad; ++i) putchar(' ');
    printf(ANSI_BOLD ANSI_MAGENTA "%s" ANSI_RESET, title);
    putchar('\n');
    for (int i = 0; i < w; ++i) putchar('=');
    putchar('\n');
}

void ui_print_banner(const char *title) {
    //ui_clear();
    ui_print("\n");
    ui_print(ANSI_BOLD ANSI_CYAN
    "  _    _                   _           _   _      _ _                          \n"
    " | |  | |                 | |         | | | |    (_) |                         \n"
    " | |__| | __ _ _   _ _ __ | |_ ___  __| | | |     _| |__  _ __ __ _ _ __ _   _ \n"
    " |  __  |/ _` | | | | '_  | __/ _    _` | | |    | | '_    '__/ _` | '__| | | |\n"
    " | |  | | (_| | |_| | | | | ||  __/ (_| | | |____| | |_) | | | (_| | |  | |_| |\n"
    " |_|  |_|  _,_|  _,_|_| |_|  _  __|  _,_| |______|_|_.__/|_|    _,_|_|     _, |\n"
    "                                                                          __/ |\n"
    "                                                                         |___/ \n"

    
"" ANSI_RESET "\n\n");
    if (title) {
        ui_boxed_title(title);
    }
    ui_print("\n");
}
