# C Programming Projects Assessment


## Projects

### Project 1: Dynamic Academic Performance and Risk Analysis Engine
Menu-driven student record management system with dynamic memory, function pointer dispatch, and a custom Academic Risk Index analysis.

### Project 2: Dynamic Text Mining and Linguistic Pattern Analyzer
Text analysis engine using pointer-based word extraction, dynamic arrays, and a custom Lexical Diversity Score analysis.

### Project 3: Dynamic IoT Event Monitoring and Callback Engine
IoT device monitoring system with callback dispatch, unions for sensor data, and a custom Sensor Drift Detector callback.

## Repository Structure

```
C_Projects_Assessment/
├── README.md
├── Project1_Academic_Performance/
│   ├── academic_engine.c
│   └── README.md
├── Project2_Text_Mining/
│   ├── text_analyzer.c
│   └── README.md
└── Project3_IoT_Monitoring/
    ├── iot_engine.c
    └── README.md
```

## How to Compile and Run

Each project is a single `.c` file. Navigate to the project directory and compile with gcc:

```bash
# Project 1
cd Project1_Academic_Performance
gcc -Wall -Wextra -std=c99 -o academic_engine academic_engine.c -lm

# Project 2
cd Project2_Text_Mining
gcc -Wall -Wextra -std=c99 -o text_analyzer text_analyzer.c

# Project 3
cd Project3_IoT_Monitoring
gcc -Wall -Wextra -std=c99 -o iot_engine iot_engine.c -lm
```
