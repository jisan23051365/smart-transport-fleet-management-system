/*
 * Smart Transport & Fleet Management System
 * ==========================================
 * C-based console application that manages vehicles, drivers, trip
 * records, and transportation cost analytics using file-based storage.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "vehicle.h"
#include "driver.h"
#include "trip.h"
#include "analytics.h"
#include "utils.h"

/* ================================================================== */
/* Forward declarations for sub-menu handlers                          */
/* ================================================================== */

static void menu_vehicles(void);
static void menu_drivers(void);
static void menu_trips(void);
static void menu_analytics(void);

/* Vehicle sub-actions */
static void vehicle_menu_list(void);
static void vehicle_menu_add(void);
static void vehicle_menu_edit(void);
static void vehicle_menu_delete(void);
static void vehicle_menu_view(void);

/* Driver sub-actions */
static void driver_menu_list(void);
static void driver_menu_add(void);
static void driver_menu_edit(void);
static void driver_menu_delete(void);
static void driver_menu_view(void);

/* Trip sub-actions */
static void trip_menu_list(void);
static void trip_menu_add(void);
static void trip_menu_edit(void);
static void trip_menu_delete(void);
static void trip_menu_view(void);
static void trip_menu_filter(void);

/* Analytics sub-actions */
static void analytics_menu_summary(void);
static void analytics_menu_by_vehicle(void);
static void analytics_menu_by_driver(void);

/* ================================================================== */
/* Main menu                                                           */
/* ================================================================== */

int main(void)
{
    utils_ensure_dir("data");

    while (1) {
        utils_clear_screen();
        utils_banner("Smart Transport & Fleet Management System");
        printf("  1. Vehicle Management\n");
        printf("  2. Driver Management\n");
        printf("  3. Trip Records\n");
        printf("  4. Transportation Cost Analytics\n");
        printf("  0. Exit\n");
        utils_separator();

        int choice = utils_input_int("Select option", 0, 4);
        switch (choice) {
            case 1: menu_vehicles();  break;
            case 2: menu_drivers();   break;
            case 3: menu_trips();     break;
            case 4: menu_analytics(); break;
            case 0:
                printf("\n  Goodbye!\n\n");
                return 0;
        }
    }
}

/* ================================================================== */
/* Vehicle Management Menu                                             */
/* ================================================================== */

static void menu_vehicles(void)
{
    while (1) {
        utils_clear_screen();
        utils_banner("Vehicle Management");
        printf("  1. List all vehicles\n");
        printf("  2. Add new vehicle\n");
        printf("  3. Edit vehicle\n");
        printf("  4. Delete vehicle\n");
        printf("  5. View vehicle details\n");
        printf("  0. Back\n");
        utils_separator();

        int choice = utils_input_int("Select option", 0, 5);
        switch (choice) {
            case 1: vehicle_menu_list();   break;
            case 2: vehicle_menu_add();    break;
            case 3: vehicle_menu_edit();   break;
            case 4: vehicle_menu_delete(); break;
            case 5: vehicle_menu_view();   break;
            case 0: return;
        }
    }
}

static void vehicle_menu_list(void)
{
    utils_clear_screen();
    utils_banner("All Vehicles");
    Vehicle buf[MAX_VEHICLES];
    int count = vehicle_load_all(buf, MAX_VEHICLES);
    if (count == 0) {
        printf("  No vehicles found.\n");
    } else {
        vehicle_print_header();
        for (int i = 0; i < count; i++)
            vehicle_print(&buf[i]);
        printf("\n  Total: %d vehicle(s)\n", count);
    }
    utils_pause();
}

static void vehicle_menu_add(void)
{
    utils_clear_screen();
    utils_banner("Add New Vehicle");

    Vehicle v;
    memset(&v, 0, sizeof(v));
    v.id = vehicle_next_id();

    utils_input_string("Make", v.make, sizeof(v.make));
    utils_input_string("Model", v.model, sizeof(v.model));
    v.year = utils_input_int("Year", 1900, 2100);
    utils_input_string("License Plate", v.license_plate, sizeof(v.license_plate));

    printf("  Status [0=Available, 1=On Trip, 2=Maintenance]: ");
    v.status = utils_input_int("Status", 0, 2);
    v.mileage = utils_input_float("Current Mileage (km)", 0);

    if (vehicle_add(&v) == 0)
        printf("\n  Vehicle added successfully (ID: %d).\n", v.id);
    else
        printf("\n  Error: could not save vehicle.\n");

    utils_pause();
}

static void vehicle_menu_edit(void)
{
    utils_clear_screen();
    utils_banner("Edit Vehicle");

    int id = utils_input_int("Enter Vehicle ID to edit", 1, 999999);
    Vehicle v;
    if (vehicle_find_by_id(id, &v) != 0) {
        printf("\n  Vehicle ID %d not found.\n", id);
        utils_pause();
        return;
    }

    printf("\n  Editing Vehicle ID %d (%s %s)\n", v.id, v.make, v.model);
    printf("  (Leave blank to keep current value)\n\n");

    char tmp[64];
    utils_input_string("Make", tmp, sizeof(tmp));
    if (tmp[0]) strncpy(v.make, tmp, sizeof(v.make) - 1);

    utils_input_string("Model", tmp, sizeof(tmp));
    if (tmp[0]) strncpy(v.model, tmp, sizeof(v.model) - 1);

    printf("  Year [current: %d, 0 to skip]: ", v.year);
    int yr = utils_input_int("Year", 0, 2100);
    if (yr > 0) v.year = yr;

    utils_input_string("License Plate", tmp, sizeof(tmp));
    if (tmp[0]) strncpy(v.license_plate, tmp, sizeof(v.license_plate) - 1);

    printf("  Status [0=Available,1=On Trip,2=Maintenance, current=%d]: ", v.status);
    int st = utils_input_int("Status", 0, 2);
    v.status = st;

    printf("  Mileage [current: %.1f, -1 to skip]: ", v.mileage);
    float ml = utils_input_float("Mileage", -1);
    if (ml >= 0) v.mileage = ml;

    if (vehicle_update(&v) == 0)
        printf("\n  Vehicle updated successfully.\n");
    else
        printf("\n  Error: could not update vehicle.\n");

    utils_pause();
}

static void vehicle_menu_delete(void)
{
    utils_clear_screen();
    utils_banner("Delete Vehicle");

    int id = utils_input_int("Enter Vehicle ID to delete", 1, 999999);
    Vehicle v;
    if (vehicle_find_by_id(id, &v) != 0) {
        printf("\n  Vehicle ID %d not found.\n", id);
        utils_pause();
        return;
    }

    printf("\n  About to delete: %s %s (%s)\n", v.make, v.model, v.license_plate);
    printf("  Confirm delete? [1=Yes, 0=No]: ");
    int confirm = utils_input_int("Confirm", 0, 1);
    if (confirm == 1) {
        if (vehicle_delete(id) == 0)
            printf("\n  Vehicle deleted successfully.\n");
        else
            printf("\n  Error: could not delete vehicle.\n");
    } else {
        printf("\n  Delete cancelled.\n");
    }
    utils_pause();
}

static void vehicle_menu_view(void)
{
    utils_clear_screen();
    utils_banner("Vehicle Details");

    int id = utils_input_int("Enter Vehicle ID", 1, 999999);
    Vehicle v;
    if (vehicle_find_by_id(id, &v) != 0) {
        printf("\n  Vehicle ID %d not found.\n", id);
        utils_pause();
        return;
    }

    utils_separator();
    printf("  ID            : %d\n",    v.id);
    printf("  Make          : %s\n",    v.make);
    printf("  Model         : %s\n",    v.model);
    printf("  Year          : %d\n",    v.year);
    printf("  License Plate : %s\n",    v.license_plate);
    printf("  Status        : ");
    vehicle_print_status(v.status);
    printf("\n");
    printf("  Mileage       : %.1f km\n", v.mileage);
    utils_separator();

    utils_pause();
}

/* ================================================================== */
/* Driver Management Menu                                              */
/* ================================================================== */

static void menu_drivers(void)
{
    while (1) {
        utils_clear_screen();
        utils_banner("Driver Management");
        printf("  1. List all drivers\n");
        printf("  2. Add new driver\n");
        printf("  3. Edit driver\n");
        printf("  4. Delete driver\n");
        printf("  5. View driver details\n");
        printf("  0. Back\n");
        utils_separator();

        int choice = utils_input_int("Select option", 0, 5);
        switch (choice) {
            case 1: driver_menu_list();   break;
            case 2: driver_menu_add();    break;
            case 3: driver_menu_edit();   break;
            case 4: driver_menu_delete(); break;
            case 5: driver_menu_view();   break;
            case 0: return;
        }
    }
}

static void driver_menu_list(void)
{
    utils_clear_screen();
    utils_banner("All Drivers");
    Driver buf[MAX_DRIVERS];
    int count = driver_load_all(buf, MAX_DRIVERS);
    if (count == 0) {
        printf("  No drivers found.\n");
    } else {
        driver_print_header();
        for (int i = 0; i < count; i++)
            driver_print(&buf[i]);
        printf("\n  Total: %d driver(s)\n", count);
    }
    utils_pause();
}

static void driver_menu_add(void)
{
    utils_clear_screen();
    utils_banner("Add New Driver");

    Driver d;
    memset(&d, 0, sizeof(d));
    d.id = driver_next_id();

    utils_input_string("Full Name", d.name, sizeof(d.name));
    utils_input_string("License Number", d.license_number, sizeof(d.license_number));
    utils_input_string("Contact Phone", d.contact, sizeof(d.contact));
    utils_input_string("Email", d.email, sizeof(d.email));

    printf("  Status [0=Active, 1=Inactive, 2=On Trip]: ");
    d.status = utils_input_int("Status", 0, 2);

    if (driver_add(&d) == 0)
        printf("\n  Driver added successfully (ID: %d).\n", d.id);
    else
        printf("\n  Error: could not save driver.\n");

    utils_pause();
}

static void driver_menu_edit(void)
{
    utils_clear_screen();
    utils_banner("Edit Driver");

    int id = utils_input_int("Enter Driver ID to edit", 1, 999999);
    Driver d;
    if (driver_find_by_id(id, &d) != 0) {
        printf("\n  Driver ID %d not found.\n", id);
        utils_pause();
        return;
    }

    printf("\n  Editing Driver ID %d (%s)\n", d.id, d.name);
    printf("  (Leave blank to keep current value)\n\n");

    char tmp[64];
    utils_input_string("Full Name", tmp, sizeof(tmp));
    if (tmp[0]) strncpy(d.name, tmp, sizeof(d.name) - 1);

    utils_input_string("License Number", tmp, sizeof(tmp));
    if (tmp[0]) strncpy(d.license_number, tmp, sizeof(d.license_number) - 1);

    utils_input_string("Contact Phone", tmp, sizeof(tmp));
    if (tmp[0]) strncpy(d.contact, tmp, sizeof(d.contact) - 1);

    utils_input_string("Email", tmp, sizeof(tmp));
    if (tmp[0]) strncpy(d.email, tmp, sizeof(d.email) - 1);

    printf("  Status [0=Active,1=Inactive,2=On Trip, current=%d]: ", d.status);
    d.status = utils_input_int("Status", 0, 2);

    if (driver_update(&d) == 0)
        printf("\n  Driver updated successfully.\n");
    else
        printf("\n  Error: could not update driver.\n");

    utils_pause();
}

static void driver_menu_delete(void)
{
    utils_clear_screen();
    utils_banner("Delete Driver");

    int id = utils_input_int("Enter Driver ID to delete", 1, 999999);
    Driver d;
    if (driver_find_by_id(id, &d) != 0) {
        printf("\n  Driver ID %d not found.\n", id);
        utils_pause();
        return;
    }

    printf("\n  About to delete driver: %s\n", d.name);
    int confirm = utils_input_int("Confirm delete? [1=Yes, 0=No]", 0, 1);
    if (confirm == 1) {
        if (driver_delete(id) == 0)
            printf("\n  Driver deleted successfully.\n");
        else
            printf("\n  Error: could not delete driver.\n");
    } else {
        printf("\n  Delete cancelled.\n");
    }
    utils_pause();
}

static void driver_menu_view(void)
{
    utils_clear_screen();
    utils_banner("Driver Details");

    int id = utils_input_int("Enter Driver ID", 1, 999999);
    Driver d;
    if (driver_find_by_id(id, &d) != 0) {
        printf("\n  Driver ID %d not found.\n", id);
        utils_pause();
        return;
    }

    utils_separator();
    printf("  ID              : %d\n",  d.id);
    printf("  Name            : %s\n",  d.name);
    printf("  License Number  : %s\n",  d.license_number);
    printf("  Contact         : %s\n",  d.contact);
    printf("  Email           : %s\n",  d.email);
    printf("  Status          : ");
    driver_print_status(d.status);
    printf("\n");
    printf("  Trips Completed : %d\n",  d.trips_completed);
    utils_separator();

    utils_pause();
}

/* ================================================================== */
/* Trip Records Menu                                                   */
/* ================================================================== */

static void menu_trips(void)
{
    while (1) {
        utils_clear_screen();
        utils_banner("Trip Records");
        printf("  1. List all trips\n");
        printf("  2. Add new trip\n");
        printf("  3. Edit trip\n");
        printf("  4. Delete trip\n");
        printf("  5. View trip details\n");
        printf("  6. Filter trips\n");
        printf("  0. Back\n");
        utils_separator();

        int choice = utils_input_int("Select option", 0, 6);
        switch (choice) {
            case 1: trip_menu_list();   break;
            case 2: trip_menu_add();    break;
            case 3: trip_menu_edit();   break;
            case 4: trip_menu_delete(); break;
            case 5: trip_menu_view();   break;
            case 6: trip_menu_filter(); break;
            case 0: return;
        }
    }
}

static void trip_menu_list(void)
{
    utils_clear_screen();
    utils_banner("All Trips");
    Trip buf[MAX_TRIPS];
    int count = trip_load_all(buf, MAX_TRIPS);
    if (count == 0) {
        printf("  No trips found.\n");
    } else {
        trip_print_header();
        for (int i = 0; i < count; i++)
            trip_print(&buf[i]);
        printf("\n  Total: %d trip(s)\n", count);
    }
    utils_pause();
}

static void trip_menu_add(void)
{
    utils_clear_screen();
    utils_banner("Add New Trip");

    Trip t;
    memset(&t, 0, sizeof(t));

    t.vehicle_id = utils_input_int("Vehicle ID", 1, 999999);
    t.driver_id  = utils_input_int("Driver ID",  1, 999999);
    utils_input_string("Origin", t.origin, sizeof(t.origin));
    utils_input_string("Destination", t.destination, sizeof(t.destination));
    t.distance_km = utils_input_float("Distance (km)", 0);

    /* Date inputs with basic validation */
    while (1) {
        utils_input_string("Departure Date (YYYY-MM-DD)", t.departure_date,
                           sizeof(t.departure_date));
        if (utils_validate_date(t.departure_date)) break;
        printf("  Invalid date format. Please use YYYY-MM-DD.\n");
    }
    while (1) {
        utils_input_string("Arrival Date (YYYY-MM-DD)", t.arrival_date,
                           sizeof(t.arrival_date));
        if (utils_validate_date(t.arrival_date)) break;
        printf("  Invalid date format. Please use YYYY-MM-DD.\n");
    }

    t.fuel_cost  = utils_input_float("Fuel Cost ($)", 0);
    t.toll_cost  = utils_input_float("Toll Cost ($)", 0);
    t.other_cost = utils_input_float("Other Cost ($)", 0);

    printf("  Status [0=Scheduled, 1=In Progress, 2=Completed, 3=Cancelled]: ");
    t.status = utils_input_int("Status", 0, 3);

    if (trip_add(&t) == 0)
        printf("\n  Trip added successfully (ID: %d, Total Cost: $%.2f).\n",
               t.id, t.total_cost);
    else
        printf("\n  Error: could not save trip.\n");

    utils_pause();
}

static void trip_menu_edit(void)
{
    utils_clear_screen();
    utils_banner("Edit Trip");

    int id = utils_input_int("Enter Trip ID to edit", 1, 999999);
    Trip t;
    if (trip_find_by_id(id, &t) != 0) {
        printf("\n  Trip ID %d not found.\n", id);
        utils_pause();
        return;
    }

    printf("\n  Editing Trip ID %d (%s -> %s)\n",
           t.id, t.origin, t.destination);
    printf("  (Leave blank / enter 0 to keep current value)\n\n");

    char tmp[64];

    utils_input_string("Origin", tmp, sizeof(tmp));
    if (tmp[0]) strncpy(t.origin, tmp, sizeof(t.origin) - 1);

    utils_input_string("Destination", tmp, sizeof(tmp));
    if (tmp[0]) strncpy(t.destination, tmp, sizeof(t.destination) - 1);

    printf("  Distance [current: %.1f, 0 to skip]: ", t.distance_km);
    float dist = utils_input_float("Distance (km)", 0);
    if (dist > 0) t.distance_km = dist;

    utils_input_string("Departure Date (YYYY-MM-DD, blank=skip)", tmp, sizeof(tmp));
    if (tmp[0] && utils_validate_date(tmp))
        strncpy(t.departure_date, tmp, sizeof(t.departure_date) - 1);

    utils_input_string("Arrival Date (YYYY-MM-DD, blank=skip)", tmp, sizeof(tmp));
    if (tmp[0] && utils_validate_date(tmp))
        strncpy(t.arrival_date, tmp, sizeof(t.arrival_date) - 1);

    printf("  Fuel Cost [current: $%.2f, -1 to skip]: ", t.fuel_cost);
    float fc = utils_input_float("Fuel Cost", -1);
    if (fc >= 0) t.fuel_cost = fc;

    printf("  Toll Cost [current: $%.2f, -1 to skip]: ", t.toll_cost);
    float tc = utils_input_float("Toll Cost", -1);
    if (tc >= 0) t.toll_cost = tc;

    printf("  Other Cost [current: $%.2f, -1 to skip]: ", t.other_cost);
    float oc = utils_input_float("Other Cost", -1);
    if (oc >= 0) t.other_cost = oc;

    printf("  Status [0=Scheduled,1=In Progress,2=Completed,3=Cancelled, current=%d]: ",
           t.status);
    t.status = utils_input_int("Status", 0, 3);

    if (trip_update(&t) == 0)
        printf("\n  Trip updated successfully (Total Cost: $%.2f).\n",
               t.total_cost);
    else
        printf("\n  Error: could not update trip.\n");

    utils_pause();
}

static void trip_menu_delete(void)
{
    utils_clear_screen();
    utils_banner("Delete Trip");

    int id = utils_input_int("Enter Trip ID to delete", 1, 999999);
    Trip t;
    if (trip_find_by_id(id, &t) != 0) {
        printf("\n  Trip ID %d not found.\n", id);
        utils_pause();
        return;
    }

    printf("\n  About to delete trip: %s -> %s\n", t.origin, t.destination);
    int confirm = utils_input_int("Confirm delete? [1=Yes, 0=No]", 0, 1);
    if (confirm == 1) {
        if (trip_delete(id) == 0)
            printf("\n  Trip deleted successfully.\n");
        else
            printf("\n  Error: could not delete trip.\n");
    } else {
        printf("\n  Delete cancelled.\n");
    }
    utils_pause();
}

static void trip_menu_view(void)
{
    utils_clear_screen();
    utils_banner("Trip Details");

    int id = utils_input_int("Enter Trip ID", 1, 999999);
    Trip t;
    if (trip_find_by_id(id, &t) != 0) {
        printf("\n  Trip ID %d not found.\n", id);
        utils_pause();
        return;
    }

    utils_separator();
    printf("  ID             : %d\n",    t.id);
    printf("  Vehicle ID     : %d\n",    t.vehicle_id);
    printf("  Driver ID      : %d\n",    t.driver_id);
    printf("  Origin         : %s\n",    t.origin);
    printf("  Destination    : %s\n",    t.destination);
    printf("  Distance       : %.1f km\n", t.distance_km);
    printf("  Departure Date : %s\n",    t.departure_date);
    printf("  Arrival Date   : %s\n",    t.arrival_date);
    printf("  Fuel Cost      : $%.2f\n", t.fuel_cost);
    printf("  Toll Cost      : $%.2f\n", t.toll_cost);
    printf("  Other Cost     : $%.2f\n", t.other_cost);
    printf("  Total Cost     : $%.2f\n", t.total_cost);
    printf("  Status         : ");
    trip_print_status(t.status);
    printf("\n");
    utils_separator();

    utils_pause();
}

static void trip_menu_filter(void)
{
    utils_clear_screen();
    utils_banner("Filter Trips");
    printf("  1. By Vehicle ID\n");
    printf("  2. By Driver ID\n");
    printf("  3. By Status\n");
    printf("  0. Back\n");
    utils_separator();

    int choice = utils_input_int("Select option", 0, 3);
    if (choice == 0) return;

    Trip buf[MAX_TRIPS];
    int count = 0;

    if (choice == 1) {
        int vid = utils_input_int("Vehicle ID", 1, 999999);
        count = trip_by_vehicle(vid, buf, MAX_TRIPS);
        printf("\n  Trips for Vehicle ID %d:\n", vid);
    } else if (choice == 2) {
        int did = utils_input_int("Driver ID", 1, 999999);
        count = trip_by_driver(did, buf, MAX_TRIPS);
        printf("\n  Trips for Driver ID %d:\n", did);
    } else {
        printf("  Status [0=Scheduled,1=In Progress,2=Completed,3=Cancelled]: ");
        int st = utils_input_int("Status", 0, 3);
        count = trip_by_status(st, buf, MAX_TRIPS);
        printf("\n  Trips with status ");
        trip_print_status(st);
        printf(":\n");
    }

    if (count == 0) {
        printf("  No trips found.\n");
    } else {
        trip_print_header();
        for (int i = 0; i < count; i++)
            trip_print(&buf[i]);
        printf("\n  Total: %d trip(s)\n", count);
    }
    utils_pause();
}

/* ================================================================== */
/* Analytics Menu                                                      */
/* ================================================================== */

static void menu_analytics(void)
{
    while (1) {
        utils_clear_screen();
        utils_banner("Transportation Cost Analytics");
        printf("  1. Overall Fleet Summary\n");
        printf("  2. Cost Breakdown by Vehicle\n");
        printf("  3. Driver Performance Report\n");
        printf("  0. Back\n");
        utils_separator();

        int choice = utils_input_int("Select option", 0, 3);
        switch (choice) {
            case 1: analytics_menu_summary();    break;
            case 2: analytics_menu_by_vehicle(); break;
            case 3: analytics_menu_by_driver();  break;
            case 0: return;
        }
    }
}

static void analytics_menu_summary(void)
{
    utils_clear_screen();
    AnalyticsSummary s = analytics_overall_summary();
    analytics_print_summary(&s);
    utils_pause();
}

static void analytics_menu_by_vehicle(void)
{
    utils_clear_screen();
    VehicleCostEntry buf[MAX_VEHICLES];
    int count = analytics_cost_by_vehicle(buf, MAX_VEHICLES);
    if (count == 0)
        printf("  No trip data available.\n");
    else
        analytics_print_vehicle_costs(buf, count);
    utils_pause();
}

static void analytics_menu_by_driver(void)
{
    utils_clear_screen();
    DriverPerformanceEntry buf[MAX_DRIVERS];
    int count = analytics_driver_performance(buf, MAX_DRIVERS);
    if (count == 0)
        printf("  No completed trips found.\n");
    else
        analytics_print_driver_performance(buf, count);
    utils_pause();
}
