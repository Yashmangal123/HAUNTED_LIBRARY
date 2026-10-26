#ifndef UI_H
#define UI_H

#include <stddef.h>

void ui_init(void);                     /* call early (main) */
void ui_clear(void);
void ui_print_banner(const char *title);/* big ASCII banner */
void ui_boxed_title(const char *title); /* centered boxed title */
void ui_print(const char *fmt, ...);    /* colored printf (default white) */
void ui_info(const char *fmt, ...);     /* cyan-ish */
void ui_warn(const char *fmt, ...);     /* yellow-ish */
void ui_error(const char *fmt, ...);    /* red */
void ui_prompt(const char *prompt);     /* print prompt (green) */
void ui_wait_for_enter(void);           /* press enter to continue */
int ui_term_width(void);                /* approximate terminal width */
void ui_clear(void);


#endif /* UI_H */
