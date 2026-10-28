// iot_engine.c - Dynamic IoT Event Monitoring and Callback Engine
// Simulates multiple IoT devices generating readings, detects abnormal
// conditions, and dispatches readings to callback functions for processing.
// Uses structures, unions, dynamic memory allocation, pointers, and function pointers.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// ---------------------------------------------------------------
// Constants
// ---------------------------------------------------------------
#define MAX_DEVICE_NAME 40
#define INITIAL_DEVICE_CAPACITY 10
#define NUM_MENU_OPTIONS 7

// Device type constants
#define DEVICE_TEMPERATURE 1
#define DEVICE_PRESSURE    2
#define DEVICE_BATTERY     3

// Operating limits for each device type
#define TEMP_MIN    -10.0
#define TEMP_MAX     45.0
#define TEMP_ALERT_LOW   0.0
#define TEMP_ALERT_HIGH  40.0

#define PRESSURE_MIN    900
#define PRESSURE_MAX    1100
#define PRESSURE_ALERT_LOW   950
#define PRESSURE_ALERT_HIGH  1050

#define VOLTAGE_MIN    2.5
#define VOLTAGE_MAX    4.2
#define VOLTAGE_ALERT_LOW   3.0
#define VOLTAGE_ALERT_HIGH  4.1

// Device status constants
#define STATUS_NORMAL  0
#define STATUS_ALERT   1

// ---------------------------------------------------------------
// Structures
// ---------------------------------------------------------------

// Union for type-specific readings - shares memory between sensor types
typedef union {
    float temperature;
    int pressure;
    float voltage;
} SensorReading;

// Device structure representing a single IoT device
typedef struct {
    int id;
    char name[MAX_DEVICE_NAME];
    int type;
    SensorReading reading;
    SensorReading prev_reading;
    int status;
    int has_previous;
    int reading_count;
} Device;

// Device database container
typedef struct {
    Device *devices;
    int count;
    int capacity;
    int total_readings;
    int normal_readings;
    int alert_readings;
    int temp_alerts;
    int pressure_alerts;
    int battery_alerts;
} DeviceDB;

// Callback function pointer type
typedef void (*DeviceCallback)(Device *);

// Menu function pointer type
typedef void (*MenuFunction)(DeviceDB *);
