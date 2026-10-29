# Project 3: Dynamic IoT Event Monitoring and Callback Engine

An IoT device monitoring system that simulates sensor readings, detects abnormal conditions through callback-based processing, and manages device registration dynamically using structures, unions, function pointers, and dynamic memory.

## Compilation

```bash
gcc -Wall -Wextra -std=c99 -o iot_engine iot_engine.c -lm
./iot_engine
```

## Features

- Register and remove IoT devices dynamically (temperature, pressure, battery)
- Simulate sensor readings with realistic random values
- Process devices through callback dispatch via `process_device()` function
- Three standard monitoring callbacks plus a custom Sensor Drift Detector
- Union-based sensor readings for type-specific data storage
- Menu-driven interface dispatched via function pointer array
