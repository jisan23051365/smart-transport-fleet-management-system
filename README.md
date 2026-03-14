# Smart Transport & Fleet Management System

A C-based console application that manages **vehicles**, **drivers**, **trip records**, and **transportation cost analytics** using file-based (binary) storage.

---

## Features

| Module | Operations |
|---|---|
| **Vehicle Management** | Add · Edit · Delete · View · List all |
| **Driver Management** | Add · Edit · Delete · View · List all |
| **Trip Records** | Add · Edit · Delete · View · List all · Filter by vehicle / driver / status |
| **Cost Analytics** | Overall fleet summary · Cost breakdown by vehicle · Driver performance report |

Data is stored in binary flat-files inside a `data/` directory (created automatically):
- `data/vehicles.dat`
- `data/drivers.dat`
- `data/trips.dat`

---

## Project Structure

```
.
├── include/          # Public header files
│   ├── vehicle.h
│   ├── driver.h
│   ├── trip.h
│   ├── analytics.h
│   └── utils.h
├── src/              # Implementation files
│   ├── main.c        # Interactive menu (entry point)
│   ├── vehicle.c
│   ├── driver.c
│   ├── trip.c
│   ├── analytics.c
│   └── utils.c
├── tests/
│   └── test_fleet.c  # Unit tests (56 assertions)
├── data/             # Created at runtime — binary data files
├── Makefile
└── README.md
```

---

## Requirements

- GCC (or any C99-compatible compiler)
- GNU Make
- POSIX-compatible OS (Linux / macOS)

---

## Build

```bash
make          # compile → produces ./fleet_mgmt
make run      # compile and launch the interactive app
make tests    # compile and run unit tests
make clean    # remove build artefacts
```

---

## Usage

Launch the application:

```bash
./fleet_mgmt
```

The main menu:

```
------------------------------------------------------------------------
           Smart Transport & Fleet Management System
------------------------------------------------------------------------
  1. Vehicle Management
  2. Driver Management
  3. Trip Records
  4. Transportation Cost Analytics
  0. Exit
------------------------------------------------------------------------
  Select option:
```

Navigate sub-menus by entering the menu numbers shown on screen.

### Typical workflow

1. **Add vehicles** – Vehicle Management → Add new vehicle  
2. **Add drivers** – Driver Management → Add new driver  
3. **Record trips** – Trip Records → Add new trip (link a vehicle ID and driver ID)  
4. **View analytics** – Transportation Cost Analytics → Overall Fleet Summary

### Trip statuses

| Value | Meaning |
|---|---|
| 0 | Scheduled |
| 1 | In Progress |
| 2 | Completed |
| 3 | Cancelled |

### Vehicle statuses

| Value | Meaning |
|---|---|
| 0 | Available |
| 1 | On Trip |
| 2 | Maintenance |

---

## Running the tests

```bash
make tests
```

Expected output:

```
Results: 56 / 56 tests passed
Results: 1 passed, 0 failed.
```

Tests cover:
- Vehicle CRUD (add / find / update / delete)
- Driver CRUD
- Trip CRUD + filtering by vehicle, driver, status
- Analytics aggregations (totals, averages, per-vehicle costs, driver performance)
- Date validation utility

---

## Data storage

Records are stored as raw C structs written with `fwrite`/`fread`.  
Maximum capacities (configurable via constants in the headers):

| Entity | Default limit |
|---|---|
| Vehicles | 500 |
| Drivers | 500 |
| Trips | 2 000 |
