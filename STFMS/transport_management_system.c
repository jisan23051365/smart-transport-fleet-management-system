#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* =========================================================
   SMART TRANSPORT & FLEET MANAGEMENT SYSTEM
   ========================================================= */

typedef struct {
    int id;
    char username[30];
    unsigned long password_hash;
} User;

typedef struct {
    int vehicle_id;
    char model[50];
    int capacity;
    float fuel_efficiency;   // km per liter
    float maintenance_cost;
} Vehicle;

typedef struct {
    int driver_id;
    char name[50];
    char license_no[30];
    float rating;
} Driver;

typedef struct {
    int trip_id;
    int vehicle_id;
    int driver_id;
    float distance;       // km
    float fuel_used;
    float trip_cost;
    char timestamp[30];
} Trip;

/* ===================== UTILITIES ===================== */

unsigned long hashPassword(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

void logActivity(const char *msg) {
    FILE *fp = fopen("transport_logs.txt", "a");
    if (!fp) return;
    time_t now = time(NULL);
    fprintf(fp, "%s - %s\n", ctime(&now), msg);
    fprintf(fp, "--------------------------------\n");
    fclose(fp);
}

/* ===================== AUTH ===================== */

void initializeAdmin() {
    FILE *fp = fopen("transport_users.dat", "rb");
    if (fp) { fclose(fp); return; }

    fp = fopen("transport_users.dat", "wb");
    User admin = {1, "admin", hashPassword("admin123")};
    fwrite(&admin, sizeof(User), 1, fp);
    fclose(fp);
}

int login() {
    char username[30], password[30];
    unsigned long hash;
    User u;

    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    hash = hashPassword(password);

    FILE *fp = fopen("transport_users.dat", "rb");
    if (!fp) return 0;

    while (fread(&u, sizeof(User), 1, fp)) {
        if (strcmp(u.username, username) == 0 && u.password_hash == hash) {
            fclose(fp);
            logActivity("Admin login successful.");
            return 1;
        }
    }

    fclose(fp);
    printf("Invalid credentials.\n");
    return 0;
}

/* ===================== VEHICLE ===================== */

void addVehicle() {
    FILE *fp = fopen("vehicles.dat", "ab");
    if (!fp) return;

    Vehicle v;
    printf("Vehicle ID: "); scanf("%d", &v.vehicle_id);
    printf("Model: "); scanf(" %[^\n]", v.model);
    printf("Capacity: "); scanf("%d", &v.capacity);
    printf("Fuel Efficiency (km per liter): "); scanf("%f", &v.fuel_efficiency);
    printf("Maintenance Cost: "); scanf("%f", &v.maintenance_cost);

    fwrite(&v, sizeof(Vehicle), 1, fp);
    fclose(fp);
    logActivity("Vehicle added.");
}

/* ===================== DRIVER ===================== */

void addDriver() {
    FILE *fp = fopen("drivers.dat", "ab");
    if (!fp) return;

    Driver d;
    printf("Driver ID: "); scanf("%d", &d.driver_id);
    printf("Name: "); scanf(" %[^\n]", d.name);
    printf("License No: "); scanf("%s", d.license_no);
    printf("Rating (0-5): "); scanf("%f", &d.rating);

    fwrite(&d, sizeof(Driver), 1, fp);
    fclose(fp);
    logActivity("Driver added.");
}

/* ===================== TRIP PROCESS ===================== */

void recordTrip() {
    FILE *vfp = fopen("vehicles.dat", "rb");
    FILE *tfp = fopen("trips.dat", "ab");

    if (!vfp || !tfp) return;

    int vehicle_id, driver_id;
    float distance;
    Vehicle v;

    printf("Vehicle ID: "); scanf("%d", &vehicle_id);
    printf("Driver ID: "); scanf("%d", &driver_id);
    printf("Distance (km): "); scanf("%f", &distance);

    while (fread(&v, sizeof(Vehicle), 1, vfp)) {
        if (v.vehicle_id == vehicle_id) {
            Trip t;
            t.trip_id = rand() % 100000;
            t.vehicle_id = vehicle_id;
            t.driver_id = driver_id;
            t.distance = distance;
            t.fuel_used = distance / v.fuel_efficiency;
            t.trip_cost = t.fuel_used * 100 + v.maintenance_cost;
            strcpy(t.timestamp, ctime(&(time_t){time(NULL)}));

            fwrite(&t, sizeof(Trip), 1, tfp);
            printf("Trip recorded. Cost: %.2f\n", t.trip_cost);
            logActivity("Trip recorded.");
            break;
        }
    }

    fclose(vfp);
    fclose(tfp);
}

/* ===================== ANALYTICS ===================== */

void tripAnalytics() {
    FILE *fp = fopen("trips.dat", "rb");
    if (!fp) return;

    Trip t;
    float arr[500];
    int n = 0;

    while (fread(&t, sizeof(Trip), 1, fp) && n < 500) {
        arr[n++] = t.trip_cost;
    }
    fclose(fp);

    if (n == 0) {
        printf("No trip data.\n");
        return;
    }

    float sum = 0;
    for (int i = 0; i < n; i++) sum += arr[i];
    float mean = sum / n;

    float variance = 0;
    for (int i = 0; i < n; i++)
        variance += pow(arr[i] - mean, 2);
    variance /= n;

    float std = sqrt(variance);

    printf("\n--- Trip Cost Analytics ---\n");
    printf("Total Trips: %d\n", n);
    printf("Average Cost: %.2f\n", mean);
    printf("Variance: %.2f\n", variance);
    printf("Standard Deviation: %.2f\n", std);

    logActivity("Trip analytics generated.");
}

/* ===================== MAIN ===================== */

int main() {
    initializeAdmin();

    printf("=== TRANSPORT & FLEET SYSTEM ===\n");

    if (!login()) return 0;

    int choice;

    while (1) {
        printf("\n1.Add Vehicle\n");
        printf("2.Add Driver\n");
        printf("3.Record Trip\n");
        printf("4.Trip Analytics\n");
        printf("5.Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addVehicle(); break;
            case 2: addDriver(); break;
            case 3: recordTrip(); break;
            case 4: tripAnalytics(); break;
            case 5:
                logActivity("System exited.");
                exit(0);
            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}