#ifndef VEHICLE_H
#define VEHICLE_H

#define MAX_VEHICLES        500
#define VEHICLE_FILE        "data/vehicles.dat"

#define VEHICLE_STATUS_AVAILABLE    0
#define VEHICLE_STATUS_ON_TRIP      1
#define VEHICLE_STATUS_MAINTENANCE  2

typedef struct {
    int  id;
    char make[32];
    char model[32];
    int  year;
    char license_plate[16];
    int  status;          /* 0=available, 1=on-trip, 2=maintenance */
    float mileage;        /* total km driven */
} Vehicle;

/* CRUD */
int  vehicle_add(const Vehicle *v);
int  vehicle_update(const Vehicle *v);
int  vehicle_delete(int id);
int  vehicle_find_by_id(int id, Vehicle *out);

/* Listing */
int  vehicle_load_all(Vehicle *buf, int buf_size);

/* Persistence */
int  vehicle_save_all(const Vehicle *buf, int count);

/* Display */
void vehicle_print_header(void);
void vehicle_print(const Vehicle *v);
void vehicle_print_status(int status);

/* Next auto-increment ID */
int  vehicle_next_id(void);

#endif /* VEHICLE_H */
