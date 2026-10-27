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

// Frees all dynamically allocated memory in the database
void destroy_database(StudentDB *db) {
    if (db != NULL) {
        if (db->records != NULL) {
            free(db->records);
            db->records = NULL;
        }
        free(db);
    }
}

// ---------------------------------------------------------------
// Utility helpers
// ---------------------------------------------------------------

// Finds a student by ID using pointer traversal, returns index or -1
int find_student_by_id(StudentDB *db, int id) {
    Student *ptr = db->records;
    int i;
    for (i = 0; i < db->count; i++, ptr++) {
        if (ptr->id == id) {
            return i;
        }
    }
    return -1;
}

// Swaps two student records using a pointer-based approach
void swap_students(Student *a, Student *b) {
    Student temp = *a;
    *a = *b;
    *b = temp;
}

// ---------------------------------------------------------------
// Core record management functions
// ---------------------------------------------------------------

// Adds a new student record with input validation
void add_student(StudentDB *db) {
    int id;
    char name[MAX_NAME_LEN];
    char course[MAX_COURSE_LEN];
    float grade;

    printf("\n--- Add New Student ---\n");

    // Get and validate student ID
    printf("Enter student ID: ");
    if (scanf("%d", &id) != 1) {
        printf("Error: Invalid ID format.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    // Check for duplicate ID
    if (find_student_by_id(db, id) != -1) {
        printf("Error: Student ID %d already exists.\n", id);
        return;
    }

    // Get student name
    printf("Enter student name: ");
    if (fgets(name, MAX_NAME_LEN, stdin) == NULL) {
        printf("Error: Failed to read name.\n");
        return;
    }
    name[strcspn(name, "\n")] = '\0';
    if (strlen(name) == 0) {
        printf("Error: Name cannot be empty.\n");
        return;
    }

    // Get course name
    printf("Enter course name: ");
    if (fgets(course, MAX_COURSE_LEN, stdin) == NULL) {
        printf("Error: Failed to read course.\n");
        return;
    }
    course[strcspn(course, "\n")] = '\0';
    if (strlen(course) == 0) {
        printf("Error: Course cannot be empty.\n");
        return;
    }

    // Get and validate grade
    printf("Enter grade (0-100): ");
    if (scanf("%f", &grade) != 1) {
        printf("Error: Invalid grade format.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (grade < 0.0 || grade > 100.0) {
        printf("Error: Grade must be between 0 and 100.\n");
        return;
    }

    // Expand capacity if needed using realloc
    if (db->count >= db->capacity) {
        int new_capacity = db->capacity * 2;
        Student *new_records = (Student *)realloc(db->records, new_capacity * sizeof(Student));
        if (new_records == NULL) {
            printf("Error: Memory reallocation failed. Cannot add student.\n");
            return;
        }
        db->records = new_records;
        db->capacity = new_capacity;
        printf("(Database expanded to capacity %d)\n", new_capacity);
    }

    // Add the new student using pointer arithmetic
    Student *new_student = db->records + db->count;
    new_student->id = id;
    strncpy(new_student->name, name, MAX_NAME_LEN - 1);
    new_student->name[MAX_NAME_LEN - 1] = '\0';
    strncpy(new_student->course, course, MAX_COURSE_LEN - 1);
    new_student->course[MAX_COURSE_LEN - 1] = '\0';
    new_student->grade = grade;

    db->count++;
    printf("Student '%s' (ID: %d) added successfully.\n", name, id);
}

// Displays all student records in a formatted table using pointer traversal
void display_students(StudentDB *db) {
    if (db->count == 0) {
        printf("\nNo student records to display.\n");
        return;
    }

    printf("\n%-6s %-25s %-20s %-8s\n", "ID", "Name", "Course", "Grade");
    printf("--------------------------------------------------------------\n");

    // Pointer-based traversal instead of array indexing
    Student *ptr = db->records;
    Student *end = db->records + db->count;
    while (ptr < end) {
        printf("%-6d %-25s %-20s %-8.1f\n",
               ptr->id, ptr->name, ptr->course, ptr->grade);
        ptr++;
    }
    printf("--------------------------------------------------------------\n");
    printf("Total records: %d\n", db->count);
}

// Sorts students by user-selected criteria
