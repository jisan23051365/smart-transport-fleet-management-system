#ifndef ANALYTICS_H
#define ANALYTICS_H

typedef struct {
    float total_cost;
    float total_fuel_cost;
    float total_toll_cost;
    float total_other_cost;
    float total_distance_km;
    float avg_cost_per_trip;
    float avg_cost_per_km;
    float avg_distance_per_trip;
    int   total_trips;
    int   completed_trips;
    int   cancelled_trips;
    int   scheduled_trips;
    int   in_progress_trips;
} AnalyticsSummary;

typedef struct {
    int   vehicle_id;
    char  vehicle_label[96];  /* "Make Model (LicPlate)" */
    float total_cost;
    float total_distance_km;
    int   trip_count;
} VehicleCostEntry;

typedef struct {
    int   driver_id;
    char  driver_name[64];
    float total_cost;
    float total_distance_km;
    int   trips_completed;
} DriverPerformanceEntry;

/* Summaries */
AnalyticsSummary analytics_overall_summary(void);
int analytics_cost_by_vehicle(VehicleCostEntry *buf, int buf_size);
int analytics_driver_performance(DriverPerformanceEntry *buf, int buf_size);

/* Display */
void analytics_print_summary(const AnalyticsSummary *s);
void analytics_print_vehicle_costs(const VehicleCostEntry *buf, int count);
void analytics_print_driver_performance(const DriverPerformanceEntry *buf, int count);

#endif /* ANALYTICS_H */
