#include <stdio.h>
#include <string.h>
#include "analytics.h"
#include "trip.h"
#include "vehicle.h"
#include "driver.h"
#include "utils.h"

/* ------------------------------------------------------------------ */
/* Overall summary                                                      */
/* ------------------------------------------------------------------ */

AnalyticsSummary analytics_overall_summary(void)
{
    AnalyticsSummary s;
    memset(&s, 0, sizeof(s));

    Trip trips[MAX_TRIPS];
    int count = trip_load_all(trips, MAX_TRIPS);

    for (int i = 0; i < count; i++) {
        s.total_trips++;
        s.total_cost        += trips[i].total_cost;
        s.total_fuel_cost   += trips[i].fuel_cost;
        s.total_toll_cost   += trips[i].toll_cost;
        s.total_other_cost  += trips[i].other_cost;
        s.total_distance_km += trips[i].distance_km;

        switch (trips[i].status) {
            case TRIP_STATUS_COMPLETED:    s.completed_trips++;   break;
            case TRIP_STATUS_CANCELLED:    s.cancelled_trips++;   break;
            case TRIP_STATUS_SCHEDULED:    s.scheduled_trips++;   break;
            case TRIP_STATUS_IN_PROGRESS:  s.in_progress_trips++; break;
        }
    }

    if (s.total_trips > 0)
        s.avg_cost_per_trip = s.total_cost / s.total_trips;
    if (s.total_distance_km > 0)
        s.avg_cost_per_km = s.total_cost / s.total_distance_km;
    if (s.total_trips > 0)
        s.avg_distance_per_trip = s.total_distance_km / s.total_trips;

    return s;
}

/* ------------------------------------------------------------------ */
/* Cost grouped by vehicle                                              */
/* ------------------------------------------------------------------ */

int analytics_cost_by_vehicle(VehicleCostEntry *buf, int buf_size)
{
    Trip trips[MAX_TRIPS];
    int trip_count = trip_load_all(trips, MAX_TRIPS);

    int n = 0;
    for (int i = 0; i < trip_count; i++) {
        int vid = trips[i].vehicle_id;
        int found = 0;
        for (int j = 0; j < n; j++) {
            if (buf[j].vehicle_id == vid) {
                buf[j].total_cost        += trips[i].total_cost;
                buf[j].total_distance_km += trips[i].distance_km;
                buf[j].trip_count++;
                found = 1;
                break;
            }
        }
        if (!found && n < buf_size) {
            Vehicle v;
            memset(&buf[n], 0, sizeof(VehicleCostEntry));
            buf[n].vehicle_id        = vid;
            buf[n].total_cost        = trips[i].total_cost;
            buf[n].total_distance_km = trips[i].distance_km;
            buf[n].trip_count        = 1;
            if (vehicle_find_by_id(vid, &v) == 0)
                snprintf(buf[n].vehicle_label, sizeof(buf[n].vehicle_label),
                         "%s %s (%s)", v.make, v.model, v.license_plate);
            else
                snprintf(buf[n].vehicle_label, sizeof(buf[n].vehicle_label),
                         "Vehicle #%d", vid);
            n++;
        }
    }
    return n;
}

/* ------------------------------------------------------------------ */
/* Driver performance                                                   */
/* ------------------------------------------------------------------ */

int analytics_driver_performance(DriverPerformanceEntry *buf, int buf_size)
{
    Trip trips[MAX_TRIPS];
    int trip_count = trip_load_all(trips, MAX_TRIPS);

    int n = 0;
    for (int i = 0; i < trip_count; i++) {
        if (trips[i].status != TRIP_STATUS_COMPLETED) continue;

        int did = trips[i].driver_id;
        int found = 0;
        for (int j = 0; j < n; j++) {
            if (buf[j].driver_id == did) {
                buf[j].total_cost        += trips[i].total_cost;
                buf[j].total_distance_km += trips[i].distance_km;
                buf[j].trips_completed++;
                found = 1;
                break;
            }
        }
        if (!found && n < buf_size) {
            Driver d;
            memset(&buf[n], 0, sizeof(DriverPerformanceEntry));
            buf[n].driver_id         = did;
            buf[n].total_cost        = trips[i].total_cost;
            buf[n].total_distance_km = trips[i].distance_km;
            buf[n].trips_completed   = 1;
            if (driver_find_by_id(did, &d) == 0)
                snprintf(buf[n].driver_name, sizeof(buf[n].driver_name),
                         "%s", d.name);
            else
                snprintf(buf[n].driver_name, sizeof(buf[n].driver_name),
                         "Driver #%d", did);
            n++;
        }
    }
    return n;
}

/* ------------------------------------------------------------------ */
/* Display                                                              */
/* ------------------------------------------------------------------ */

void analytics_print_summary(const AnalyticsSummary *s)
{
    utils_banner("Fleet Analytics – Overall Summary");
    printf("  Total Trips          : %d\n",    s->total_trips);
    printf("  Completed            : %d\n",    s->completed_trips);
    printf("  In Progress          : %d\n",    s->in_progress_trips);
    printf("  Scheduled            : %d\n",    s->scheduled_trips);
    printf("  Cancelled            : %d\n",    s->cancelled_trips);
    utils_separator();
    printf("  Total Distance       : %.1f km\n",  s->total_distance_km);
    printf("  Avg Distance/Trip    : %.1f km\n",  s->avg_distance_per_trip);
    utils_separator();
    printf("  Total Cost           : $%.2f\n", s->total_cost);
    printf("    Fuel               : $%.2f\n", s->total_fuel_cost);
    printf("    Toll               : $%.2f\n", s->total_toll_cost);
    printf("    Other              : $%.2f\n", s->total_other_cost);
    printf("  Avg Cost/Trip        : $%.2f\n", s->avg_cost_per_trip);
    printf("  Avg Cost/km          : $%.4f\n", s->avg_cost_per_km);
    utils_separator();
}

void analytics_print_vehicle_costs(const VehicleCostEntry *buf, int count)
{
    utils_banner("Fleet Analytics – Cost by Vehicle");
    printf("%-5s %-40s %8s %10s %10s\n",
           "ID", "Vehicle", "Trips", "Dist(km)", "TotalCost");
    utils_separator();
    for (int i = 0; i < count; i++)
        printf("%-5d %-40s %8d %10.1f %10.2f\n",
               buf[i].vehicle_id, buf[i].vehicle_label,
               buf[i].trip_count, buf[i].total_distance_km,
               buf[i].total_cost);
    utils_separator();
}

void analytics_print_driver_performance(const DriverPerformanceEntry *buf, int count)
{
    utils_banner("Fleet Analytics – Driver Performance (completed trips)");
    printf("%-5s %-30s %8s %10s %10s\n",
           "ID", "Driver", "Trips", "Dist(km)", "TotalCost");
    utils_separator();
    for (int i = 0; i < count; i++)
        printf("%-5d %-30s %8d %10.1f %10.2f\n",
               buf[i].driver_id, buf[i].driver_name,
               buf[i].trips_completed, buf[i].total_distance_km,
               buf[i].total_cost);
    utils_separator();
}
