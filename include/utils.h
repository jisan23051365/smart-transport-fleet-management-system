#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

/* Clear terminal screen */
void utils_clear_screen(void);

/* Press-enter-to-continue prompt */
void utils_pause(void);

/* Safe string input: reads up to (len-1) chars, strips newline */
void utils_input_string(const char *prompt, char *buf, size_t len);

/* Safe integer input with optional range check (pass min > max to skip) */
int  utils_input_int(const char *prompt, int min, int max);

/* Safe float input */
float utils_input_float(const char *prompt, float min);

/* Validate date string YYYY-MM-DD; returns 1 if valid */
int  utils_validate_date(const char *date);

/* Ensure directory exists (creates if needed) */
int  utils_ensure_dir(const char *path);

/* Horizontal separator line */
void utils_separator(void);

/* Print a centred title banner */
void utils_banner(const char *title);

#endif /* UTILS_H */
