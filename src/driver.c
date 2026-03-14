#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver.h"
#include "utils.h"

/* ------------------------------------------------------------------ */
/* Persistence helpers                                                  */
/* ------------------------------------------------------------------ */

int driver_load_all(Driver *buf, int buf_size)
{
    FILE *fp = fopen(DRIVER_FILE, "rb");
    if (!fp) return 0;

    int count = 0;
    while (count < buf_size &&
           fread(&buf[count], sizeof(Driver), 1, fp) == 1)
        count++;
    fclose(fp);
    return count;
}

int driver_save_all(const Driver *buf, int count)
{
    utils_ensure_dir("data");
    FILE *fp = fopen(DRIVER_FILE, "wb");
    if (!fp) return -1;
    fwrite(buf, sizeof(Driver), count, fp);
    fclose(fp);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Auto-increment ID                                                    */
/* ------------------------------------------------------------------ */

int driver_next_id(void)
{
    Driver buf[MAX_DRIVERS];
    int count = driver_load_all(buf, MAX_DRIVERS);
    int max_id = 0;
    for (int i = 0; i < count; i++)
        if (buf[i].id > max_id) max_id = buf[i].id;
    return max_id + 1;
}

/* ------------------------------------------------------------------ */
/* CRUD                                                                 */
/* ------------------------------------------------------------------ */

int driver_add(const Driver *d)
{
    Driver buf[MAX_DRIVERS];
    int count = driver_load_all(buf, MAX_DRIVERS);
    if (count >= MAX_DRIVERS) return -1;
    buf[count] = *d;
    return driver_save_all(buf, count + 1);
}

int driver_update(const Driver *d)
{
    Driver buf[MAX_DRIVERS];
    int count = driver_load_all(buf, MAX_DRIVERS);
    for (int i = 0; i < count; i++) {
        if (buf[i].id == d->id) {
            buf[i] = *d;
            return driver_save_all(buf, count);
        }
    }
    return -1;
}

int driver_delete(int id)
{
    Driver buf[MAX_DRIVERS];
    int count = driver_load_all(buf, MAX_DRIVERS);
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (buf[i].id == id) {
            memmove(&buf[i], &buf[i + 1],
                    (count - i - 1) * sizeof(Driver));
            count--;
            found = 1;
            break;
        }
    }
    if (!found) return -1;
    return driver_save_all(buf, count);
}

int driver_find_by_id(int id, Driver *out)
{
    Driver buf[MAX_DRIVERS];
    int count = driver_load_all(buf, MAX_DRIVERS);
    for (int i = 0; i < count; i++) {
        if (buf[i].id == id) {
            *out = buf[i];
            return 0;
        }
    }
    return -1;
}

/* ------------------------------------------------------------------ */
/* Display                                                              */
/* ------------------------------------------------------------------ */

void driver_print_status(int status)
{
    switch (status) {
        case DRIVER_STATUS_ACTIVE:   printf("Active");   break;
        case DRIVER_STATUS_INACTIVE: printf("Inactive"); break;
        case DRIVER_STATUS_ON_TRIP:  printf("On Trip");  break;
        default:                     printf("Unknown");  break;
    }
}

void driver_print_header(void)
{
    printf("%-5s %-24s %-16s %-16s %-10s %8s\n",
           "ID", "Name", "License No.", "Contact", "Status", "Trips");
    utils_separator();
}

void driver_print(const Driver *d)
{
    printf("%-5d %-24s %-16s %-16s %-10s %8d\n",
           d->id, d->name, d->license_number, d->contact,
           (d->status == DRIVER_STATUS_ACTIVE)   ? "Active"   :
           (d->status == DRIVER_STATUS_INACTIVE)  ? "Inactive" :
           (d->status == DRIVER_STATUS_ON_TRIP)   ? "On Trip"  : "Unknown",
           d->trips_completed);
}
