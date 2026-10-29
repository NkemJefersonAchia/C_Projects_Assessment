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
void process_device(Device *device, void (*callback)(Device *)) {
    callback(device);
}

// Temperature monitoring callback
void temperature_monitor(Device *device) {
    float temp = device->reading.temperature;

    printf("  [Temperature Monitor] Device %d (%s): %.1f C ",
           device->id, device->name, temp);

    if (temp < TEMP_ALERT_LOW || temp > TEMP_ALERT_HIGH) {
        device->status = STATUS_ALERT;
        printf("-> ALERT!");
        if (temp < TEMP_ALERT_LOW)
            printf(" (Below minimum %.1f C)", TEMP_ALERT_LOW);
        else
            printf(" (Exceeds maximum %.1f C)", TEMP_ALERT_HIGH);
    } else {
        device->status = STATUS_NORMAL;
        printf("-> Normal");
    }
    printf("\n");
}

// Pressure monitoring callback
void pressure_monitor(Device *device) {
    int pressure = device->reading.pressure;

    printf("  [Pressure Monitor] Device %d (%s): %d hPa ",
           device->id, device->name, pressure);

    if (pressure < PRESSURE_ALERT_LOW || pressure > PRESSURE_ALERT_HIGH) {
        device->status = STATUS_ALERT;
        printf("-> ALERT!");
        if (pressure < PRESSURE_ALERT_LOW)
            printf(" (Below minimum %d hPa)", PRESSURE_ALERT_LOW);
        else
            printf(" (Exceeds maximum %d hPa)", PRESSURE_ALERT_HIGH);
    } else {
        device->status = STATUS_NORMAL;
        printf("-> Normal");
    }
    printf("\n");
}

// Battery voltage monitoring callback
void battery_monitor(Device *device) {
    float voltage = device->reading.voltage;

    printf("  [Battery Monitor] Device %d (%s): %.2f V ",
           device->id, device->name, voltage);

    if (voltage < VOLTAGE_ALERT_LOW || voltage > VOLTAGE_ALERT_HIGH) {
        device->status = STATUS_ALERT;
        printf("-> ALERT!");
        if (voltage < VOLTAGE_ALERT_LOW)
            printf(" (Below minimum %.1f V - LOW BATTERY)", VOLTAGE_ALERT_LOW);
        else
            printf(" (Exceeds maximum %.1f V - OVERCHARGE)", VOLTAGE_ALERT_HIGH);
    } else {
        device->status = STATUS_NORMAL;
        printf("-> Normal");
    }
    printf("\n");
}

// Custom Callback: Sensor Drift Detector
// Detects significant changes between consecutive readings from the same device
// Thresholds: Temperature > 10 C, Pressure > 30 hPa, Battery > 0.5 V
void sensor_drift_detector(Device *device) {
    if (!device->has_previous) {
        printf("  [Drift Detector] Device %d (%s): First reading - "
               "no drift data yet.\n", device->id, device->name);
        return;
    }

    float drift = 0.0;
    float threshold = 0.0;
    const char *unit = "";

    // Calculate drift based on device type, accessing correct union member
    switch (device->type) {
        case DEVICE_TEMPERATURE:
            drift = fabs(device->reading.temperature - device->prev_reading.temperature);
            threshold = 10.0;
            unit = "C";
            printf("  [Drift Detector] Device %d (%s): Drift = %.1f %s ",
                   device->id, device->name, drift, unit);
            break;
        case DEVICE_PRESSURE:
            drift = abs(device->reading.pressure - device->prev_reading.pressure);
            threshold = 30.0;
            unit = "hPa";
            printf("  [Drift Detector] Device %d (%s): Drift = %.0f %s ",
                   device->id, device->name, drift, unit);
            break;
        case DEVICE_BATTERY:
            drift = fabs(device->reading.voltage - device->prev_reading.voltage);
            threshold = 0.5;
            unit = "V";
            printf("  [Drift Detector] Device %d (%s): Drift = %.2f %s ",
                   device->id, device->name, drift, unit);
            break;
    }

    if (drift > threshold) {
        printf("-> DRIFT ALERT! (threshold: %.1f %s)\n", threshold, unit);
    } else {
        printf("-> Stable (within threshold)\n");
    }
}

// Returns the appropriate callback for a given device type
DeviceCallback get_callback_for_type(int type) {
    switch (type) {
        case DEVICE_TEMPERATURE: return temperature_monitor;
        case DEVICE_PRESSURE:    return pressure_monitor;
        case DEVICE_BATTERY:     return battery_monitor;
        default:                 return NULL;
    }
}

// ---------------------------------------------------------------
// Device management operations
// ---------------------------------------------------------------

// Registers a new device with input validation
void register_device(DeviceDB *db) {
    int id, type;
    char name[MAX_DEVICE_NAME];

    printf("\n--- Register New Device ---\n");

    printf("Enter device ID: ");
    if (scanf("%d", &id) != 1) {
        printf("Error: Invalid ID format.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (find_device_by_id(db, id) != -1) {
        printf("Error: Device ID %d already exists.\n", id);
        return;
    }

    printf("Enter device name: ");
    if (fgets(name, MAX_DEVICE_NAME, stdin) == NULL) {
        printf("Error: Failed to read name.\n");
        return;
    }
    name[strcspn(name, "\n")] = '\0';
    if (strlen(name) == 0) {
        printf("Error: Name cannot be empty.\n");
        return;
    }

    printf("Select device type:\n");
    printf("  1. Temperature Sensor\n");
    printf("  2. Pressure Sensor\n");
    printf("  3. Battery Voltage Monitor\n");
    printf("Enter type (1-3): ");
    if (scanf("%d", &type) != 1 || type < 1 || type > 3) {
        printf("Error: Invalid device type.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    // Expand array if at capacity using realloc
    if (db->count >= db->capacity) {
        int new_capacity = db->capacity * 2;
        Device *new_devices = (Device *)realloc(db->devices,
                                                  new_capacity * sizeof(Device));
        if (new_devices == NULL) {
            printf("Error: Memory reallocation failed.\n");
            return;
        }
        db->devices = new_devices;
        db->capacity = new_capacity;
        printf("(Device array expanded to capacity %d)\n", new_capacity);
    }

    // Initialize the new device using pointer arithmetic
    Device *new_dev = db->devices + db->count;
    new_dev->id = id;
    strncpy(new_dev->name, name, MAX_DEVICE_NAME - 1);
    new_dev->name[MAX_DEVICE_NAME - 1] = '\0';
    new_dev->type = type;
    new_dev->status = STATUS_NORMAL;
    new_dev->has_previous = 0;
    new_dev->reading_count = 0;

    // Initialize the appropriate union member based on type
    switch (type) {
        case DEVICE_TEMPERATURE: new_dev->reading.temperature = 0.0; break;
        case DEVICE_PRESSURE:    new_dev->reading.pressure = 0; break;
        case DEVICE_BATTERY:     new_dev->reading.voltage = 0.0; break;
    }

    db->count++;
    printf("Device '%s' (ID: %d, Type: %s) registered.\n",
           name, id, get_type_name(type));
}

// Displays all registered devices in a formatted table
void display_devices(DeviceDB *db) {
    if (db->count == 0) {
        printf("\nNo devices registered.\n");
        return;
    }

    printf("\n%-6s %-25s %-15s %-15s %-10s\n",
           "ID", "Name", "Type", "Last Reading", "Status");
    printf("----------------------------------------------------------------------\n");

    // Pointer-based traversal of device array
    Device *ptr = db->devices;
    Device *end = db->devices + db->count;
    while (ptr < end) {
        char reading_str[30];

        // Access the correct union member based on device type
        if (ptr->reading_count == 0) {
            strcpy(reading_str, "No reading");
        } else {
            switch (ptr->type) {
                case DEVICE_TEMPERATURE:
                    sprintf(reading_str, "%.1f C", ptr->reading.temperature);
                    break;
                case DEVICE_PRESSURE:
                    sprintf(reading_str, "%d hPa", ptr->reading.pressure);
                    break;
                case DEVICE_BATTERY:
                    sprintf(reading_str, "%.2f V", ptr->reading.voltage);
                    break;
                default:
                    strcpy(reading_str, "N/A");
            }
        }

        printf("%-6d %-25s %-15s %-15s %-10s\n",
               ptr->id, ptr->name, get_type_name(ptr->type),
               reading_str,
               ptr->status == STATUS_NORMAL ? "NORMAL" : "ALERT");
        ptr++;
    }
    printf("----------------------------------------------------------------------\n");
    printf("Total devices: %d\n", db->count);
}

// Removes a device by ID and shrinks the array
void remove_device(DeviceDB *db) {
    if (db->count == 0) {
        printf("\nNo devices to remove.\n");
        return;
    }

    int id;
    printf("\n--- Remove Device ---\n");
    printf("Enter device ID to remove: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid ID format.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    int index = find_device_by_id(db, id);
    if (index == -1) {
        printf("Device with ID %d not found.\n", id);
        return;
    }

    char removed_name[MAX_DEVICE_NAME];
    strncpy(removed_name, (db->devices + index)->name, MAX_DEVICE_NAME - 1);
    removed_name[MAX_DEVICE_NAME - 1] = '\0';

    // Shift remaining devices left
    Device *target = db->devices + index;
    Device *source = target + 1;
    int remaining = db->count - index - 1;
    if (remaining > 0) {
        memmove(target, source, remaining * sizeof(Device));
    }

    db->count--;
    printf("Device '%s' (ID: %d) removed.\n", removed_name, id);

    // Shrink if count drops below quarter capacity
    if (db->count > 0 && db->count < db->capacity / 4
        && db->capacity > INITIAL_DEVICE_CAPACITY) {
        int new_cap = db->capacity / 2;
        if (new_cap < INITIAL_DEVICE_CAPACITY) new_cap = INITIAL_DEVICE_CAPACITY;
        Device *new_devs = (Device *)realloc(db->devices, new_cap * sizeof(Device));
        if (new_devs != NULL) {
            db->devices = new_devs;
            db->capacity = new_cap;
            printf("(Device array shrunk to capacity %d)\n", new_cap);
        }
    }
}

// ---------------------------------------------------------------
// Simulation and reporting
// ---------------------------------------------------------------

// Generates simulated readings for all registered devices
void generate_readings(DeviceDB *db) {
    if (db->count == 0) {
        printf("\nNo devices registered. Register devices first.\n");
        return;
    }

    printf("\n========================================================\n");
    printf("       GENERATING SIMULATED READINGS\n");
    printf("========================================================\n");

    Device *ptr = db->devices;
    Device *end = db->devices + db->count;
    int reading_num = 1;

    while (ptr < end) {
        // Save previous reading before generating new one
        if (ptr->reading_count > 0) {
            ptr->prev_reading = ptr->reading;
            ptr->has_previous = 1;
        }

        printf("\nReading #%d:\n", reading_num);

        // Generate reading based on device type, writing to correct union member
        switch (ptr->type) {
            case DEVICE_TEMPERATURE:
                ptr->reading.temperature = random_float(TEMP_MIN, TEMP_MAX);
                break;
            case DEVICE_PRESSURE:
                ptr->reading.pressure = random_int(PRESSURE_MIN, PRESSURE_MAX);
                break;
            case DEVICE_BATTERY:
                ptr->reading.voltage = random_float(VOLTAGE_MIN, VOLTAGE_MAX);
                break;
        }

        ptr->reading_count++;

        // Get the appropriate callback using function pointer dispatch
        DeviceCallback callback = get_callback_for_type(ptr->type);
        if (callback != NULL) {
            process_device(ptr, callback);

            db->total_readings++;
            if (ptr->status == STATUS_ALERT) {
                db->alert_readings++;
                switch (ptr->type) {
                    case DEVICE_TEMPERATURE: db->temp_alerts++; break;
                    case DEVICE_PRESSURE: db->pressure_alerts++; break;
                    case DEVICE_BATTERY: db->battery_alerts++; break;
                }
            } else {
                db->normal_readings++;
            }

            // Also run custom drift detector callback via function pointer
            process_device(ptr, sensor_drift_detector);
        }

        reading_num++;
        ptr++;
    }

    printf("\n========================================================\n");
    printf("Generated %d readings for %d devices.\n", db->count, db->count);
}

// Displays all currently active alerts
