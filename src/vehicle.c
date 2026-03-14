#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vehicle.h"
#include "utils.h"

/* ------------------------------------------------------------------ */
/* Persistence helpers                                                  */
/* ------------------------------------------------------------------ */

int vehicle_load_all(Vehicle *buf, int buf_size)
{
    FILE *fp = fopen(VEHICLE_FILE, "rb");
    if (!fp) return 0;

    int count = 0;
    while (count < buf_size &&
           fread(&buf[count], sizeof(Vehicle), 1, fp) == 1)
        count++;
    fclose(fp);
    return count;
}

int vehicle_save_all(const Vehicle *buf, int count)
{
    utils_ensure_dir("data");
    FILE *fp = fopen(VEHICLE_FILE, "wb");
    if (!fp) return -1;
    fwrite(buf, sizeof(Vehicle), count, fp);
    fclose(fp);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Auto-increment ID                                                    */
/* ------------------------------------------------------------------ */

int vehicle_next_id(void)
{
    Vehicle buf[MAX_VEHICLES];
    int count = vehicle_load_all(buf, MAX_VEHICLES);
    int max_id = 0;
    for (int i = 0; i < count; i++)
        if (buf[i].id > max_id) max_id = buf[i].id;
    return max_id + 1;
}

/* ------------------------------------------------------------------ */
/* CRUD                                                                 */
/* ------------------------------------------------------------------ */

int vehicle_add(const Vehicle *v)
{
    Vehicle buf[MAX_VEHICLES];
    int count = vehicle_load_all(buf, MAX_VEHICLES);
    if (count >= MAX_VEHICLES) return -1;
    buf[count] = *v;
    return vehicle_save_all(buf, count + 1);
}

int vehicle_update(const Vehicle *v)
{
    Vehicle buf[MAX_VEHICLES];
    int count = vehicle_load_all(buf, MAX_VEHICLES);
    for (int i = 0; i < count; i++) {
        if (buf[i].id == v->id) {
            buf[i] = *v;
            return vehicle_save_all(buf, count);
        }
    }
    return -1; /* not found */
}

int vehicle_delete(int id)
{
    Vehicle buf[MAX_VEHICLES];
    int count = vehicle_load_all(buf, MAX_VEHICLES);
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (buf[i].id == id) {
            memmove(&buf[i], &buf[i + 1],
                    (count - i - 1) * sizeof(Vehicle));
            count--;
            found = 1;
            break;
        }
    }
    if (!found) return -1;
    return vehicle_save_all(buf, count);
}

int vehicle_find_by_id(int id, Vehicle *out)
{
    Vehicle buf[MAX_VEHICLES];
    int count = vehicle_load_all(buf, MAX_VEHICLES);
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

void vehicle_print_status(int status)
{
    switch (status) {
        case VEHICLE_STATUS_AVAILABLE:   printf("Available");   break;
        case VEHICLE_STATUS_ON_TRIP:     printf("On Trip");     break;
        case VEHICLE_STATUS_MAINTENANCE: printf("Maintenance"); break;
        default:                         printf("Unknown");     break;
    }
}

void vehicle_print_header(void)
{
    printf("%-5s %-14s %-14s %-6s %-12s %-12s %10s\n",
           "ID", "Make", "Model", "Year", "Plate", "Status", "Mileage(km)");
    utils_separator();
}

void vehicle_print(const Vehicle *v)
{
    printf("%-5d %-14s %-14s %-6d %-12s %-12s %10.1f\n",
           v->id, v->make, v->model, v->year,
           v->license_plate,
           (v->status == VEHICLE_STATUS_AVAILABLE)   ? "Available"   :
           (v->status == VEHICLE_STATUS_ON_TRIP)      ? "On Trip"     :
           (v->status == VEHICLE_STATUS_MAINTENANCE)  ? "Maintenance" : "Unknown",
           v->mileage);
}
