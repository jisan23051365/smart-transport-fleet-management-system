#ifndef TRIP_H
#define TRIP_H

#define MAX_TRIPS           2000
#define TRIP_FILE           "data/trips.dat"

#define TRIP_STATUS_SCHEDULED   0
#define TRIP_STATUS_IN_PROGRESS 1
#define TRIP_STATUS_COMPLETED   2
#define TRIP_STATUS_CANCELLED   3

typedef struct {
    int   id;
    int   vehicle_id;
    int   driver_id;
    char  origin[64];
    char  destination[64];
    float distance_km;
    char  departure_date[12];   /* YYYY-MM-DD */
    char  arrival_date[12];     /* YYYY-MM-DD */
    float fuel_cost;
    float toll_cost;
    float other_cost;
    float total_cost;           /* computed: fuel+toll+other */
    int   status;               /* 0=scheduled,1=in-progress,2=completed,3=cancelled */
} Trip;

/* CRUD */
int  trip_add(Trip *t);             /* sets id and total_cost */
int  trip_update(Trip *t);          /* recomputes total_cost  */
int  trip_delete(int id);
int  trip_find_by_id(int id, Trip *out);

/* Filtering */
int  trip_load_all(Trip *buf, int buf_size);
int  trip_by_vehicle(int vehicle_id, Trip *buf, int buf_size);
int  trip_by_driver(int driver_id, Trip *buf, int buf_size);
int  trip_by_status(int status, Trip *buf, int buf_size);

/* Persistence */
int  trip_save_all(const Trip *buf, int count);

/* Display */
void trip_print_header(void);
void trip_print(const Trip *t);
void trip_print_status(int status);

/* Next auto-increment ID */
int  trip_next_id(void);

#endif /* TRIP_H */
