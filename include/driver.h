#ifndef DRIVER_H
#define DRIVER_H

#define MAX_DRIVERS         500
#define DRIVER_FILE         "data/drivers.dat"

#define DRIVER_STATUS_ACTIVE    0
#define DRIVER_STATUS_INACTIVE  1
#define DRIVER_STATUS_ON_TRIP   2

typedef struct {
    int  id;
    char name[64];
    char license_number[24];
    char contact[20];
    char email[48];
    int  status;          /* 0=active, 1=inactive, 2=on-trip */
    int  trips_completed;
} Driver;

/* CRUD */
int  driver_add(const Driver *d);
int  driver_update(const Driver *d);
int  driver_delete(int id);
int  driver_find_by_id(int id, Driver *out);

/* Listing */
int  driver_load_all(Driver *buf, int buf_size);

/* Persistence */
int  driver_save_all(const Driver *buf, int count);

/* Display */
void driver_print_header(void);
void driver_print(const Driver *d);
void driver_print_status(int status);

/* Next auto-increment ID */
int  driver_next_id(void);

#endif /* DRIVER_H */
