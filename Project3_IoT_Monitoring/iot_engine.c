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

// ---------------------------------------------------------------
// Database lifecycle
// ---------------------------------------------------------------

// Creates a new device database with dynamic allocation
DeviceDB *create_device_db(int initial_capacity) {
    DeviceDB *db = (DeviceDB *)malloc(sizeof(DeviceDB));
    if (db == NULL) {
        printf("Error: Failed to allocate device database.\n");
        return NULL;
    }

    db->devices = (Device *)calloc(initial_capacity, sizeof(Device));
    if (db->devices == NULL) {
        printf("Error: Failed to allocate device array.\n");
        free(db);
        return NULL;
    }

    db->count = 0;
    db->capacity = initial_capacity;
    db->total_readings = 0;
    db->normal_readings = 0;
    db->alert_readings = 0;
    db->temp_alerts = 0;
    db->pressure_alerts = 0;
    db->battery_alerts = 0;

    return db;
}

// Frees all dynamically allocated memory
void destroy_device_db(DeviceDB *db) {
    if (db != NULL) {
        if (db->devices != NULL) free(db->devices);
        free(db);
    }
}

// ---------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------

// Returns string name for a device type
const char *get_type_name(int type) {
    switch (type) {
        case DEVICE_TEMPERATURE: return "Temperature";
        case DEVICE_PRESSURE:    return "Pressure";
        case DEVICE_BATTERY:     return "Battery";
        default:                 return "Unknown";
    }
}

// Finds a device by ID using pointer traversal, returns index or -1
int find_device_by_id(DeviceDB *db, int id) {
    Device *ptr = db->devices;
    int i;
    for (i = 0; i < db->count; i++, ptr++) {
        if (ptr->id == id) return i;
    }
    return -1;
}

// Generates a random float in [min, max] range
float random_float(float min, float max) {
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

// Generates a random integer in [min, max] range
int random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

// ---------------------------------------------------------------
// Callback functions
// ---------------------------------------------------------------

// Generic callback dispatcher - processes a device through its callback
