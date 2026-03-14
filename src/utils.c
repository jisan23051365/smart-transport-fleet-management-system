#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "utils.h"

/* ------------------------------------------------------------------ */
/* Terminal helpers                                                     */
/* ------------------------------------------------------------------ */

void utils_clear_screen(void)
{
#ifdef _WIN32
    system("cls");
#else
    /* Use ANSI escape instead of system("clear") to avoid
       running an external process. */
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

void utils_pause(void)
{
    printf("\n  Press ENTER to continue...");
    fflush(stdout);
    /* Discard characters until newline */
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/* ------------------------------------------------------------------ */
/* Input helpers                                                        */
/* ------------------------------------------------------------------ */

void utils_input_string(const char *prompt, char *buf, size_t len)
{
    printf("  %s: ", prompt);
    fflush(stdout);
    if (fgets(buf, (int)len, stdin) != NULL) {
        size_t slen = strlen(buf);
        if (slen > 0 && buf[slen - 1] == '\n')
            buf[slen - 1] = '\0';
    } else {
        buf[0] = '\0';
    }
}

int utils_input_int(const char *prompt, int min, int max)
{
    int value;
    int valid_range = (min <= max);
    while (1) {
        printf("  %s: ", prompt);
        fflush(stdout);
        int r = scanf("%d", &value);
        if (r == EOF) return min; /* stdin closed */
        if (r == 1) {
            /* discard rest of line */
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            if (!valid_range || (value >= min && value <= max))
                return value;
            printf("  Please enter a value between %d and %d.\n", min, max);
        } else {
            /* discard invalid input */
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            printf("  Invalid input. Please enter a number.\n");
        }
    }
}

float utils_input_float(const char *prompt, float min)
{
    float value;
    while (1) {
        printf("  %s: ", prompt);
        fflush(stdout);
        int r = scanf("%f", &value);
        if (r == EOF) return min >= 0 ? min : 0.0f; /* stdin closed */
        if (r == 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            if (value >= min)
                return value;
            printf("  Value must be >= %.2f\n", min);
        } else {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            printf("  Invalid input. Please enter a number.\n");
        }
    }
}

/* ------------------------------------------------------------------ */
/* Date validation                                                      */
/* ------------------------------------------------------------------ */

int utils_validate_date(const char *date)
{
    int y, m, d;
    if (sscanf(date, "%d-%d-%d", &y, &m, &d) != 3) return 0;
    if (strlen(date) != 10)                          return 0;
    if (m < 1 || m > 12)                             return 0;
    if (d < 1 || d > 31)                             return 0;
    if (y < 1900 || y > 2200)                        return 0;
    return 1;
}

/* ------------------------------------------------------------------ */
/* Directory helper                                                     */
/* ------------------------------------------------------------------ */

int utils_ensure_dir(const char *path)
{
#ifdef _WIN32
    return _mkdir(path);
#else
    return mkdir(path, 0755);
#endif
}

/* ------------------------------------------------------------------ */
/* Visual helpers                                                       */
/* ------------------------------------------------------------------ */

void utils_separator(void)
{
    printf("------------------------------------------------------------------------"
           "--------\n");
}

void utils_banner(const char *title)
{
    utils_separator();
    int len = (int)strlen(title);
    int pad = (78 - len) / 2;
    if (pad < 0) pad = 0;
    printf("%*s%s\n", pad, "", title);
    utils_separator();
}
