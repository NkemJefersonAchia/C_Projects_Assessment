// academic_engine.c - Dynamic Academic Performance and Risk Analysis Engine
// A menu-driven student record management system that evaluates performance,
// identifies at-risk students, and generates statistical summaries.
// Uses structures, dynamic memory allocation, pointers, and function pointers.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ---------------------------------------------------------------
// Constants
// ---------------------------------------------------------------
#define MAX_NAME_LEN 50
#define MAX_COURSE_LEN 50
#define INITIAL_CAPACITY 15
#define PASS_THRESHOLD 50.0
#define NUM_MENU_OPTIONS 9

// Risk level thresholds for the Academic Risk Index
#define RISK_HIGH_THRESHOLD 75.0
#define RISK_ELEVATED_THRESHOLD 55.0
#define RISK_MODERATE_THRESHOLD 30.0

// ---------------------------------------------------------------
// Structures
// ---------------------------------------------------------------

// Student record structure
typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char course[MAX_COURSE_LEN];
    float grade;
} Student;

// Database container for dynamic student management
typedef struct {
    Student *records;
    int count;
    int capacity;
} StudentDB;

// Function pointer type for menu operations
typedef void (*MenuFunction)(StudentDB *);

// ---------------------------------------------------------------
// Database lifecycle functions
// ---------------------------------------------------------------

// Creates and initializes a new student database with given capacity
StudentDB *create_database(int initial_capacity) {
    StudentDB *db = (StudentDB *)malloc(sizeof(StudentDB));
    if (db == NULL) {
        printf("Error: Failed to allocate memory for database.\n");
        return NULL;
    }

    db->records = (Student *)malloc(initial_capacity * sizeof(Student));
    if (db->records == NULL) {
        printf("Error: Failed to allocate memory for student records.\n");
        free(db);
        return NULL;
    }

    db->count = 0;
    db->capacity = initial_capacity;
    return db;
}

