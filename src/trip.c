#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trip.h"
#include "utils.h"

/* ------------------------------------------------------------------ */
/* Persistence helpers                                                  */
/* ------------------------------------------------------------------ */

int trip_load_all(Trip *buf, int buf_size)
{
    FILE *fp = fopen(TRIP_FILE, "rb");
    if (!fp) return 0;

    int count = 0;
    while (count < buf_size &&
           fread(&buf[count], sizeof(Trip), 1, fp) == 1)
        count++;
    fclose(fp);
    return count;
}

int trip_save_all(const Trip *buf, int count)
{
    utils_ensure_dir("data");
    FILE *fp = fopen(TRIP_FILE, "wb");
    if (!fp) return -1;
    fwrite(buf, sizeof(Trip), count, fp);
    fclose(fp);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Auto-increment ID                                                    */
/* ------------------------------------------------------------------ */

int trip_next_id(void)
{
    Trip buf[MAX_TRIPS];
    int count = trip_load_all(buf, MAX_TRIPS);
    int max_id = 0;
    for (int i = 0; i < count; i++)
        if (buf[i].id > max_id) max_id = buf[i].id;
    return max_id + 1;
}

/* ------------------------------------------------------------------ */
/* CRUD                                                                 */
/* ------------------------------------------------------------------ */

int trip_add(Trip *t)
{
    Trip buf[MAX_TRIPS];
    int count = trip_load_all(buf, MAX_TRIPS);
    if (count >= MAX_TRIPS) return -1;

    t->id = trip_next_id();
    t->total_cost = t->fuel_cost + t->toll_cost + t->other_cost;

    buf[count] = *t;
    return trip_save_all(buf, count + 1);
}

int trip_update(Trip *t)
{
    Trip buf[MAX_TRIPS];
    int count = trip_load_all(buf, MAX_TRIPS);

    t->total_cost = t->fuel_cost + t->toll_cost + t->other_cost;

    for (int i = 0; i < count; i++) {
        if (buf[i].id == t->id) {
            buf[i] = *t;
            return trip_save_all(buf, count);
        }
    }
    return -1;
}

int trip_delete(int id)
{
    Trip buf[MAX_TRIPS];
    int count = trip_load_all(buf, MAX_TRIPS);
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (buf[i].id == id) {
            memmove(&buf[i], &buf[i + 1],
                    (count - i - 1) * sizeof(Trip));
            count--;
            found = 1;
            break;
        }
    }
    if (!found) return -1;
    return trip_save_all(buf, count);
}

int trip_find_by_id(int id, Trip *out)
{
    Trip buf[MAX_TRIPS];
    int count = trip_load_all(buf, MAX_TRIPS);
    for (int i = 0; i < count; i++) {
        if (buf[i].id == id) {
            *out = buf[i];
            return 0;
        }
    }
    return -1;
}

/* ------------------------------------------------------------------ */
/* Filtering                                                            */
/* ------------------------------------------------------------------ */

int trip_by_vehicle(int vehicle_id, Trip *buf, int buf_size)
{
    Trip all[MAX_TRIPS];
    int total = trip_load_all(all, MAX_TRIPS);
    int count = 0;
    for (int i = 0; i < total && count < buf_size; i++)
        if (all[i].vehicle_id == vehicle_id)
            buf[count++] = all[i];
    return count;
}

int trip_by_driver(int driver_id, Trip *buf, int buf_size)
{
    Trip all[MAX_TRIPS];
    int total = trip_load_all(all, MAX_TRIPS);
    int count = 0;
    for (int i = 0; i < total && count < buf_size; i++)
        if (all[i].driver_id == driver_id)
            buf[count++] = all[i];
    return count;
}

int trip_by_status(int status, Trip *buf, int buf_size)
{
    Trip all[MAX_TRIPS];
    int total = trip_load_all(all, MAX_TRIPS);
    int count = 0;
    for (int i = 0; i < total && count < buf_size; i++)
        if (all[i].status == status)
            buf[count++] = all[i];
    return count;
}

/* ------------------------------------------------------------------ */
/* Display                                                              */
/* ------------------------------------------------------------------ */

void trip_print_status(int status)
{
    switch (status) {
        case TRIP_STATUS_SCHEDULED:    printf("Scheduled");    break;
        case TRIP_STATUS_IN_PROGRESS:  printf("In Progress"); break;
        case TRIP_STATUS_COMPLETED:    printf("Completed");   break;
        case TRIP_STATUS_CANCELLED:    printf("Cancelled");   break;
        default:                       printf("Unknown");     break;
    }
}

void trip_print_header(void)
{
    printf("%-5s %-5s %-5s %-20s %-20s %8s %-12s %-12s %10s %-12s\n",
           "ID", "VehID", "DrvID", "Origin", "Destination",
           "Dist(km)", "Departure", "Arrival", "TotalCost", "Status");
    utils_separator();
}

void trip_print(const Trip *t)
{
    printf("%-5d %-5d %-5d %-20s %-20s %8.1f %-12s %-12s %10.2f %-12s\n",
           t->id, t->vehicle_id, t->driver_id,
           t->origin, t->destination,
           t->distance_km,
           t->departure_date, t->arrival_date,
           t->total_cost,
           (t->status == TRIP_STATUS_SCHEDULED)   ? "Scheduled"   :
           (t->status == TRIP_STATUS_IN_PROGRESS)  ? "In Progress" :
           (t->status == TRIP_STATUS_COMPLETED)    ? "Completed"   :
           (t->status == TRIP_STATUS_CANCELLED)    ? "Cancelled"   : "Unknown");
}
