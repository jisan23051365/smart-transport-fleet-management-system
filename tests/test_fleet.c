/*
 * Unit tests for the Smart Transport & Fleet Management System.
 * Tests run against in-memory logic; temporary data directory is
 * created in /tmp to avoid polluting the project's data/.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>

/* All modules under test */
#include "vehicle.h"
#include "driver.h"
#include "trip.h"
#include "analytics.h"
#include "utils.h"

/* ------------------------------------------------------------------ */
/* Helper: redirect data files to a temporary directory               */
/* ------------------------------------------------------------------ */

#define TMP_DATA_DIR "/tmp/fleet_test_data"

/* Override the file paths by changing the working directory so that
   the relative paths used in the modules resolve to /tmp. */
static void setup_tmp_dir(void)
{
    mkdir(TMP_DATA_DIR, 0755);
    /* Create the data sub-directory inside tmp dir */
    char data_path[256];
    snprintf(data_path, sizeof(data_path), "%s/data", TMP_DATA_DIR);
    mkdir(data_path, 0755);
    chdir(TMP_DATA_DIR);
}

/* Remove all .dat files to start fresh */
static void cleanup_data(void)
{
    remove(VEHICLE_FILE);
    remove(DRIVER_FILE);
    remove(TRIP_FILE);
}

/* ------------------------------------------------------------------ */
/* Assertion counter                                                   */
/* ------------------------------------------------------------------ */

static int tests_run    = 0;
static int tests_passed = 0;

#define TEST_ASSERT(cond, msg)                                 \
    do {                                                        \
        tests_run++;                                           \
        if (cond) {                                            \
            tests_passed++;                                    \
            printf("  [PASS] %s\n", msg);                      \
        } else {                                               \
            printf("  [FAIL] %s  (line %d)\n", msg, __LINE__); \
        }                                                      \
    } while (0)

/* ================================================================== */
/* Vehicle tests                                                       */
/* ================================================================== */

static void test_vehicle_crud(void)
{
    printf("\n--- Vehicle CRUD ---\n");
    cleanup_data();

    /* next id on empty store should be 1 */
    TEST_ASSERT(vehicle_next_id() == 1, "next_id on empty store == 1");

    Vehicle v;
    memset(&v, 0, sizeof(v));
    v.id = vehicle_next_id();
    strncpy(v.make,          "Toyota",   sizeof(v.make) - 1);
    strncpy(v.model,         "Camry",    sizeof(v.model) - 1);
    strncpy(v.license_plate, "ABC-123",  sizeof(v.license_plate) - 1);
    v.year    = 2020;
    v.status  = VEHICLE_STATUS_AVAILABLE;
    v.mileage = 15000.0f;

    TEST_ASSERT(vehicle_add(&v) == 0, "vehicle_add returns 0");

    Vehicle loaded;
    TEST_ASSERT(vehicle_find_by_id(1, &loaded) == 0, "vehicle_find_by_id == 0");
    TEST_ASSERT(strcmp(loaded.make, "Toyota") == 0, "loaded make == Toyota");
    TEST_ASSERT(loaded.year == 2020,                "loaded year == 2020");
    TEST_ASSERT(loaded.mileage == 15000.0f,         "loaded mileage == 15000");

    /* update */
    loaded.mileage = 20000.0f;
    TEST_ASSERT(vehicle_update(&loaded) == 0, "vehicle_update returns 0");

    Vehicle updated;
    vehicle_find_by_id(1, &updated);
    TEST_ASSERT(updated.mileage == 20000.0f, "mileage updated to 20000");

    /* add a second vehicle */
    Vehicle v2;
    memset(&v2, 0, sizeof(v2));
    v2.id = vehicle_next_id();
    TEST_ASSERT(v2.id == 2, "second vehicle gets id 2");
    strncpy(v2.make, "Ford", sizeof(v2.make) - 1);
    strncpy(v2.model, "Transit", sizeof(v2.model) - 1);
    v2.year = 2019;
    vehicle_add(&v2);

    Vehicle buf[MAX_VEHICLES];
    int count = vehicle_load_all(buf, MAX_VEHICLES);
    TEST_ASSERT(count == 2, "load_all returns 2 vehicles");

    /* delete first vehicle */
    TEST_ASSERT(vehicle_delete(1) == 0, "vehicle_delete id 1 returns 0");
    count = vehicle_load_all(buf, MAX_VEHICLES);
    TEST_ASSERT(count == 1, "load_all returns 1 after delete");
    TEST_ASSERT(buf[0].id == 2, "remaining vehicle has id 2");

    /* delete non-existent */
    TEST_ASSERT(vehicle_delete(99) == -1, "delete non-existent returns -1");
}

/* ================================================================== */
/* Driver tests                                                        */
/* ================================================================== */

static void test_driver_crud(void)
{
    printf("\n--- Driver CRUD ---\n");
    cleanup_data();

    TEST_ASSERT(driver_next_id() == 1, "driver next_id on empty store == 1");

    Driver d;
    memset(&d, 0, sizeof(d));
    d.id = 1;
    strncpy(d.name,           "Alice Johnson", sizeof(d.name) - 1);
    strncpy(d.license_number, "LIC-001",       sizeof(d.license_number) - 1);
    strncpy(d.contact,        "555-1234",      sizeof(d.contact) - 1);
    d.status          = DRIVER_STATUS_ACTIVE;
    d.trips_completed = 5;

    TEST_ASSERT(driver_add(&d) == 0, "driver_add returns 0");

    Driver loaded;
    TEST_ASSERT(driver_find_by_id(1, &loaded) == 0,    "driver_find_by_id == 0");
    TEST_ASSERT(strcmp(loaded.name, "Alice Johnson") == 0, "loaded name correct");
    TEST_ASSERT(loaded.trips_completed == 5,            "trips_completed == 5");

    /* update */
    loaded.trips_completed = 10;
    TEST_ASSERT(driver_update(&loaded) == 0, "driver_update returns 0");
    Driver updated;
    driver_find_by_id(1, &updated);
    TEST_ASSERT(updated.trips_completed == 10, "trips_completed updated to 10");

    /* delete */
    TEST_ASSERT(driver_delete(1) == 0,  "driver_delete returns 0");
    TEST_ASSERT(driver_find_by_id(1, &loaded) == -1, "find after delete returns -1");
}

/* ================================================================== */
/* Trip tests                                                          */
/* ================================================================== */

static void test_trip_crud(void)
{
    printf("\n--- Trip CRUD ---\n");
    cleanup_data();

    Trip t;
    memset(&t, 0, sizeof(t));
    t.vehicle_id  = 1;
    t.driver_id   = 1;
    strncpy(t.origin,         "CityA", sizeof(t.origin) - 1);
    strncpy(t.destination,    "CityB", sizeof(t.destination) - 1);
    strncpy(t.departure_date, "2024-01-10", sizeof(t.departure_date) - 1);
    strncpy(t.arrival_date,   "2024-01-11", sizeof(t.arrival_date) - 1);
    t.distance_km = 300.0f;
    t.fuel_cost   = 50.0f;
    t.toll_cost   = 10.0f;
    t.other_cost  = 5.0f;
    t.status      = TRIP_STATUS_COMPLETED;

    TEST_ASSERT(trip_add(&t) == 0, "trip_add returns 0");
    TEST_ASSERT(t.id == 1,         "trip id assigned as 1");
    TEST_ASSERT(t.total_cost == 65.0f, "total_cost = fuel+toll+other = 65");

    Trip loaded;
    TEST_ASSERT(trip_find_by_id(1, &loaded) == 0, "trip_find_by_id == 0");
    TEST_ASSERT(loaded.total_cost == 65.0f,       "loaded total_cost == 65");
    TEST_ASSERT(strcmp(loaded.origin, "CityA") == 0, "loaded origin == CityA");

    /* update */
    loaded.other_cost = 15.0f;
    TEST_ASSERT(trip_update(&loaded) == 0, "trip_update returns 0");
    Trip updated;
    trip_find_by_id(1, &updated);
    TEST_ASSERT(updated.total_cost == 75.0f, "updated total_cost == 75");

    /* add a second trip with different vehicle */
    Trip t2;
    memset(&t2, 0, sizeof(t2));
    t2.vehicle_id = 2;
    t2.driver_id  = 1;
    strncpy(t2.origin,         "CityC", sizeof(t2.origin) - 1);
    strncpy(t2.destination,    "CityD", sizeof(t2.destination) - 1);
    strncpy(t2.departure_date, "2024-02-01", sizeof(t2.departure_date) - 1);
    strncpy(t2.arrival_date,   "2024-02-02", sizeof(t2.arrival_date) - 1);
    t2.distance_km = 200.0f;
    t2.fuel_cost   = 30.0f;
    t2.status      = TRIP_STATUS_SCHEDULED;
    trip_add(&t2);
    TEST_ASSERT(t2.id == 2, "second trip id == 2");

    /* filter by vehicle */
    Trip fbuf[MAX_TRIPS];
    int fc = trip_by_vehicle(1, fbuf, MAX_TRIPS);
    TEST_ASSERT(fc == 1, "trip_by_vehicle(1) returns 1 trip");

    fc = trip_by_driver(1, fbuf, MAX_TRIPS);
    TEST_ASSERT(fc == 2, "trip_by_driver(1) returns 2 trips");

    fc = trip_by_status(TRIP_STATUS_COMPLETED, fbuf, MAX_TRIPS);
    TEST_ASSERT(fc == 1, "trip_by_status(COMPLETED) returns 1 trip");

    /* delete */
    TEST_ASSERT(trip_delete(1) == 0, "trip_delete returns 0");
    Trip buf[MAX_TRIPS];
    int cnt = trip_load_all(buf, MAX_TRIPS);
    TEST_ASSERT(cnt == 1, "load_all after delete returns 1 trip");
}

/* ================================================================== */
/* Analytics tests                                                     */
/* ================================================================== */

static void test_analytics(void)
{
    printf("\n--- Analytics ---\n");
    cleanup_data();

    /* Seed two vehicles */
    Vehicle v1, v2;
    memset(&v1, 0, sizeof(v1));
    v1.id = 1; strncpy(v1.make, "Toyota", sizeof(v1.make)-1);
    strncpy(v1.model, "Camry", sizeof(v1.model)-1);
    strncpy(v1.license_plate, "T-001", sizeof(v1.license_plate)-1);
    vehicle_add(&v1);

    memset(&v2, 0, sizeof(v2));
    v2.id = 2; strncpy(v2.make, "Ford", sizeof(v2.make)-1);
    strncpy(v2.model, "Transit", sizeof(v2.model)-1);
    strncpy(v2.license_plate, "F-002", sizeof(v2.license_plate)-1);
    vehicle_add(&v2);

    /* Seed one driver */
    Driver d;
    memset(&d, 0, sizeof(d));
    d.id = 1; strncpy(d.name, "Bob Smith", sizeof(d.name)-1);
    driver_add(&d);

    /* Seed three trips */
    Trip t;
    memset(&t, 0, sizeof(t));
    t.vehicle_id = 1; t.driver_id = 1;
    t.fuel_cost = 100.0f; t.toll_cost = 20.0f; t.other_cost = 10.0f;
    t.distance_km = 500.0f;
    t.status = TRIP_STATUS_COMPLETED;
    strncpy(t.departure_date, "2024-01-01", sizeof(t.departure_date)-1);
    strncpy(t.arrival_date,   "2024-01-02", sizeof(t.arrival_date)-1);
    trip_add(&t); /* total = 130, id=1 */

    memset(&t, 0, sizeof(t));
    t.vehicle_id = 1; t.driver_id = 1;
    t.fuel_cost = 80.0f; t.toll_cost = 0.0f; t.other_cost = 5.0f;
    t.distance_km = 300.0f;
    t.status = TRIP_STATUS_COMPLETED;
    strncpy(t.departure_date, "2024-02-01", sizeof(t.departure_date)-1);
    strncpy(t.arrival_date,   "2024-02-02", sizeof(t.arrival_date)-1);
    trip_add(&t); /* total = 85, id=2 */

    memset(&t, 0, sizeof(t));
    t.vehicle_id = 2; t.driver_id = 1;
    t.fuel_cost = 60.0f; t.toll_cost = 5.0f; t.other_cost = 0.0f;
    t.distance_km = 150.0f;
    t.status = TRIP_STATUS_CANCELLED;
    strncpy(t.departure_date, "2024-03-01", sizeof(t.departure_date)-1);
    strncpy(t.arrival_date,   "2024-03-01", sizeof(t.arrival_date)-1);
    trip_add(&t); /* total = 65, id=3 */

    AnalyticsSummary s = analytics_overall_summary();
    TEST_ASSERT(s.total_trips       == 3,     "total_trips == 3");
    TEST_ASSERT(s.completed_trips   == 2,     "completed_trips == 2");
    TEST_ASSERT(s.cancelled_trips   == 1,     "cancelled_trips == 1");
    TEST_ASSERT(s.total_cost        == 280.0f,"total_cost == 280");
    TEST_ASSERT(s.total_distance_km == 950.0f,"total_distance == 950");

    float expected_avg = 280.0f / 3.0f;
    TEST_ASSERT(s.avg_cost_per_trip >= expected_avg - 0.01f &&
                s.avg_cost_per_trip <= expected_avg + 0.01f,
                "avg_cost_per_trip correct");

    /* Cost by vehicle */
    VehicleCostEntry vbuf[MAX_VEHICLES];
    int vc = analytics_cost_by_vehicle(vbuf, MAX_VEHICLES);
    TEST_ASSERT(vc == 2, "cost_by_vehicle returns 2 entries");

    float v1_cost = 0, v2_cost = 0;
    for (int i = 0; i < vc; i++) {
        if (vbuf[i].vehicle_id == 1) v1_cost = vbuf[i].total_cost;
        if (vbuf[i].vehicle_id == 2) v2_cost = vbuf[i].total_cost;
    }
    TEST_ASSERT(v1_cost == 215.0f, "vehicle 1 total cost == 215");
    TEST_ASSERT(v2_cost == 65.0f,  "vehicle 2 total cost == 65");

    /* Driver performance (completed trips only) */
    DriverPerformanceEntry dbuf[MAX_DRIVERS];
    int dc = analytics_driver_performance(dbuf, MAX_DRIVERS);
    TEST_ASSERT(dc == 1,                    "driver_performance returns 1 entry");
    TEST_ASSERT(dbuf[0].trips_completed == 2, "driver 1 completed 2 trips");
    TEST_ASSERT(dbuf[0].total_cost == 215.0f, "driver 1 total cost == 215");
}

/* ================================================================== */
/* Utility tests                                                       */
/* ================================================================== */

static void test_utils(void)
{
    printf("\n--- Utility helpers ---\n");
    TEST_ASSERT(utils_validate_date("2024-01-15") == 1, "valid date 2024-01-15");
    TEST_ASSERT(utils_validate_date("2024-13-01") == 0, "month 13 is invalid");
    TEST_ASSERT(utils_validate_date("2024-00-10") == 0, "month 0 is invalid");
    TEST_ASSERT(utils_validate_date("2024-01-00") == 0, "day 0 is invalid");
    TEST_ASSERT(utils_validate_date("2024-01-32") == 0, "day 32 is invalid");
    TEST_ASSERT(utils_validate_date("2024/01/15") == 0, "wrong separator");
    TEST_ASSERT(utils_validate_date("24-01-15")   == 0, "short year");
}

/* ================================================================== */
/* main                                                                */
/* ================================================================== */

int main(void)
{
    printf("=======================================================\n");
    printf("  Smart Transport Fleet Management – Unit Tests\n");
    printf("=======================================================\n");

    setup_tmp_dir();

    test_vehicle_crud();
    test_driver_crud();
    test_trip_crud();
    test_analytics();
    test_utils();

    printf("\n=======================================================\n");
    printf("  Results: %d / %d tests passed\n", tests_passed, tests_run);
    printf("=======================================================\n\n");

    return (tests_passed == tests_run) ? 0 : 1;
}
