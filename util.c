#include "util.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void to_lower(char *s) {
    for (; *s; ++s) *s = (char)tolower((unsigned char)*s);
}
void strip_nl(char *s) { s[strcspn(s, "\n")] = '\0'; }

int parse_int(const char *s, int *out) {
    if (!s || !*s) return 0;
    char *end; long v = strtol(s, &end, 10);
    if (end == s || *end) return 0;
    *out = (int)v; return 1;
}
