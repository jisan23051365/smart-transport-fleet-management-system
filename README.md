# Smart Transport & Fleet Management System

## Overview
The Smart Transport & Fleet Management System is a console-based transportation management application written in C. The system simulates fleet operations including vehicle management, driver records, trip tracking, and transportation cost analytics.

The system also includes authentication, activity logging, and file-based storage to maintain fleet and trip records.

## Features

### Authentication System
- Admin login with username and password
- Password hashing for security
- Automatic administrator initialization

### Vehicle Management
- Add vehicles to the fleet
- Store vehicle capacity, fuel efficiency, and maintenance cost
- Maintain vehicle operational records

### Driver Management
- Register drivers
- Store driver license information
- Track driver ratings

### Trip Management
- Record trips with vehicle and driver details
- Calculate fuel consumption automatically
- Estimate trip cost based on fuel usage and maintenance cost
- Store trip records with timestamps

### Trip Cost Analytics
The system analyzes trip data and provides statistical insights including:

- Total number of trips
- Average trip cost
- Variance
- Standard deviation

### Activity Logging
All system operations are logged including:

- Admin login
- Vehicle registration
- Driver registration
- Trip recording
- Analytics generation
- System exit

## Technologies Used
- C Programming Language
- Standard C Libraries
- File-based persistent storage

## System Data Files

| File | Purpose |
|-----|-----|
| transport_users.dat | Authentication records |
| vehicles.dat | Vehicle fleet data |
| drivers.dat | Driver records |
| trips.dat | Trip transaction records |
| transport_logs.txt | System activity logs |

## Default Admin Login

Username: admin  
Password: admin123

## Program Menu

1. Add Vehicle  
2. Add Driver  
3. Record Trip  
4. Trip Analytics  
5. Exit  

## Learning Objectives

This project demonstrates:

- Fleet management system simulation
- Transportation logistics tracking
- File handling in C
- Authentication using hashing
- Cost estimation algorithms
- Statistical analytics
- Console-based transportation management applications

## Author
Jisan
