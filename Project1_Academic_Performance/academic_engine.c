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
void sort_students(StudentDB *db) {
    if (db->count < 2) {
        printf("\nNot enough records to sort.\n");
        return;
    }

    int choice;
    printf("\n--- Sort Students ---\n");
    printf("1. Sort by Grade (descending)\n");
    printf("2. Sort by Name (alphabetical)\n");
    printf("3. Sort by Student ID (ascending)\n");
    printf("Enter choice: ");

    if (scanf("%d", &choice) != 1 || choice < 1 || choice > 3) {
        printf("Invalid sort option.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    // Bubble sort with pointer-based traversal
    int i, j;
    for (i = 0; i < db->count - 1; i++) {
        for (j = 0; j < db->count - i - 1; j++) {
            Student *current = db->records + j;
            Student *next = db->records + j + 1;
            int should_swap = 0;

            switch (choice) {
                case 1:
                    should_swap = (current->grade < next->grade);
                    break;
                case 2:
                    should_swap = (strcmp(current->name, next->name) > 0);
                    break;
                case 3:
                    should_swap = (current->id > next->id);
                    break;
            }

            if (should_swap) {
                swap_students(current, next);
            }
        }
    }

    const char *sort_labels[] = {"", "Grade (descending)", "Name (alphabetical)", "ID (ascending)"};
    printf("Students sorted by %s.\n", sort_labels[choice]);
    display_students(db);
}

// Calculates and displays the average grade using pointer arithmetic
void calculate_average(StudentDB *db) {
    if (db->count == 0) {
        printf("\nNo student records available.\n");
        return;
    }

    float total = 0.0;
    Student *ptr = db->records;
    int i;

    for (i = 0; i < db->count; i++) {
        total += (ptr + i)->grade;
    }

    float average = total / db->count;
    printf("\nAverage grade across %d students: %.2f\n", db->count, average);
}

// Finds and displays the top-performing student using pointer traversal
void find_top_student(StudentDB *db) {
    if (db->count == 0) {
        printf("\nNo student records available.\n");
        return;
    }

    Student *top = db->records;
    Student *ptr = db->records + 1;
    Student *end = db->records + db->count;

    while (ptr < end) {
        if (ptr->grade > top->grade) {
            top = ptr;
        }
        ptr++;
    }

    printf("\n--- Top Performing Student ---\n");
    printf("ID:     %d\n", top->id);
    printf("Name:   %s\n", top->name);
    printf("Course: %s\n", top->course);
    printf("Grade:  %.1f\n", top->grade);
}

// Searches for a student by ID or name
void search_student(StudentDB *db) {
    if (db->count == 0) {
        printf("\nNo student records available.\n");
        return;
    }

    int choice;
    printf("\n--- Search Student ---\n");
    printf("1. Search by ID\n");
    printf("2. Search by Name\n");
    printf("Enter choice: ");

    if (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2)) {
        printf("Invalid search option.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (choice == 1) {
        int id;
        printf("Enter student ID: ");
        if (scanf("%d", &id) != 1) {
            printf("Invalid ID format.\n");
            while (getchar() != '\n');
            return;
        }
        while (getchar() != '\n');

        int index = find_student_by_id(db, id);
        if (index == -1) {
            printf("Student with ID %d not found.\n", id);
        } else {
            Student *s = db->records + index;
            printf("\nStudent Found:\n");
            printf("ID: %d | Name: %s | Course: %s | Grade: %.1f\n",
                   s->id, s->name, s->course, s->grade);
        }
    } else {
        char search_name[MAX_NAME_LEN];
        printf("Enter student name: ");
        if (fgets(search_name, MAX_NAME_LEN, stdin) == NULL) {
            printf("Error reading input.\n");
            return;
        }
        search_name[strcspn(search_name, "\n")] = '\0';

        int found = 0;
        Student *ptr = db->records;
        int i;
        for (i = 0; i < db->count; i++, ptr++) {
            if (strcmp(ptr->name, search_name) == 0) {
                printf("\nStudent Found:\n");
                printf("ID: %d | Name: %s | Course: %s | Grade: %.1f\n",
                       ptr->id, ptr->name, ptr->course, ptr->grade);
                found = 1;
            }
        }

        if (!found) {
            printf("No student with name '%s' found.\n", search_name);
        }
    }
}

// Removes a student record by ID and shrinks memory if possible
void remove_student(StudentDB *db) {
    if (db->count == 0) {
        printf("\nNo student records to remove.\n");
        return;
    }

    int id;
    printf("\n--- Remove Student ---\n");
    printf("Enter student ID to remove: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid ID format.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    int index = find_student_by_id(db, id);
    if (index == -1) {
        printf("Student with ID %d not found.\n", id);
        return;
    }

    char removed_name[MAX_NAME_LEN];
    strncpy(removed_name, (db->records + index)->name, MAX_NAME_LEN - 1);
    removed_name[MAX_NAME_LEN - 1] = '\0';

    // Shift remaining records left using pointer arithmetic
    Student *target = db->records + index;
    Student *source = target + 1;
    int remaining = db->count - index - 1;
    if (remaining > 0) {
        memmove(target, source, remaining * sizeof(Student));
    }

    db->count--;
    printf("Student '%s' (ID: %d) removed successfully.\n", removed_name, id);

    // Shrink capacity if count drops below a quarter of capacity
    if (db->count > 0 && db->count < db->capacity / 4 && db->capacity > INITIAL_CAPACITY) {
        int new_capacity = db->capacity / 2;
        if (new_capacity < INITIAL_CAPACITY) {
            new_capacity = INITIAL_CAPACITY;
        }
        Student *new_records = (Student *)realloc(db->records, new_capacity * sizeof(Student));
        if (new_records != NULL) {
            db->records = new_records;
            db->capacity = new_capacity;
            printf("(Database shrunk to capacity %d)\n", new_capacity);
        }
    }
}

// ---------------------------------------------------------------
// Custom Analysis: Academic Risk Index
// ---------------------------------------------------------------

// Calculates the risk score for a single student
// Formula: risk_score = (100 - grade) * weight_factor
// weight_factor increases when grade is below class average
float calculate_risk_score(float grade, float class_average) {
    float base_risk = 100.0 - grade;
    float weight_factor;

    if (grade < class_average) {
        float deviation = class_average - grade;
        weight_factor = 1.0 + (deviation / class_average);
    } else {
        weight_factor = 0.8;
    }

    float risk_score = base_risk * weight_factor;

    // Clamp between 0 and 100
    if (risk_score < 0.0) risk_score = 0.0;
    if (risk_score > 100.0) risk_score = 100.0;

    return risk_score;
}

// Runs the Academic Risk Index analysis on all students
void custom_analysis(StudentDB *db) {
    if (db->count == 0) {
        printf("\nNo student records available for analysis.\n");
        return;
    }

    // Calculate class average first
    float total = 0.0;
    Student *ptr = db->records;
    int i;
    for (i = 0; i < db->count; i++) {
        total += (ptr + i)->grade;
    }
    float class_average = total / db->count;

    printf("\n========================================================\n");
    printf("       ACADEMIC RISK INDEX ANALYSIS\n");
    printf("========================================================\n");
    printf("Class Average: %.2f\n", class_average);
    printf("Risk Calculation: risk = (100 - grade) * weight_factor\n");
    printf("  Weight factor is amplified when grade < class average\n");
    printf("  Risk Levels: Low (0-30), Moderate (30-55),\n");
    printf("               Elevated (55-75), High (75+)\n");
    printf("--------------------------------------------------------\n");
    printf("%-6s %-20s %-8s %-10s %-12s\n",
           "ID", "Name", "Grade", "Risk Score", "Risk Level");
    printf("--------------------------------------------------------\n");

    int high_risk = 0, elevated_risk = 0, moderate_risk = 0, low_risk = 0;

    ptr = db->records;
    for (i = 0; i < db->count; i++, ptr++) {
        float risk = calculate_risk_score(ptr->grade, class_average);
        const char *level;

        if (risk >= RISK_HIGH_THRESHOLD) {
            level = "HIGH";
            high_risk++;
        } else if (risk >= RISK_ELEVATED_THRESHOLD) {
            level = "ELEVATED";
            elevated_risk++;
        } else if (risk >= RISK_MODERATE_THRESHOLD) {
            level = "MODERATE";
            moderate_risk++;
        } else {
            level = "LOW";
            low_risk++;
        }

        printf("%-6d %-20s %-8.1f %-10.2f %-12s\n",
               ptr->id, ptr->name, ptr->grade, risk, level);
    }

    printf("--------------------------------------------------------\n");
    printf("Risk Distribution:\n");
    printf("  High Risk:     %d students (%.1f%%)\n",
           high_risk, (float)high_risk / db->count * 100);
    printf("  Elevated Risk: %d students (%.1f%%)\n",
           elevated_risk, (float)elevated_risk / db->count * 100);
    printf("  Moderate Risk: %d students (%.1f%%)\n",
           moderate_risk, (float)moderate_risk / db->count * 100);
    printf("  Low Risk:      %d students (%.1f%%)\n",
           low_risk, (float)low_risk / db->count * 100);
    printf("========================================================\n");

    if (high_risk > 0) {
        printf("\nRECOMMENDATION: %d student(s) require immediate academic support.\n",
               high_risk);
    }
}

// ---------------------------------------------------------------
// Summary Report
// ---------------------------------------------------------------

// Generates a comprehensive summary report of all student data
void generate_summary_report(StudentDB *db) {
    if (db->count == 0) {
        printf("\nNo student records available for summary.\n");
        return;
    }

    float total = 0.0;
    float highest = db->records->grade;
    float lowest = db->records->grade;
    int pass_count = 0;
    int fail_count = 0;
    char top_name[MAX_NAME_LEN];
    char low_name[MAX_NAME_LEN];

    strncpy(top_name, db->records->name, MAX_NAME_LEN - 1);
    strncpy(low_name, db->records->name, MAX_NAME_LEN - 1);

    Student *ptr = db->records;
    int i;
    for (i = 0; i < db->count; i++, ptr++) {
        total += ptr->grade;

        if (ptr->grade > highest) {
            highest = ptr->grade;
            strncpy(top_name, ptr->name, MAX_NAME_LEN - 1);
        }
        if (ptr->grade < lowest) {
            lowest = ptr->grade;
            strncpy(low_name, ptr->name, MAX_NAME_LEN - 1);
        }

        if (ptr->grade >= PASS_THRESHOLD) {
            pass_count++;
        } else {
            fail_count++;
        }
    }

    float average = total / db->count;

    printf("\n========================================================\n");
    printf("           ACADEMIC PERFORMANCE SUMMARY REPORT\n");
    printf("========================================================\n");
    printf("Total students:          %d\n", db->count);
    printf("Average grade:           %.2f\n", average);
    printf("Highest grade:           %.1f (%s)\n", highest, top_name);
    printf("Lowest grade:            %.1f (%s)\n", lowest, low_name);
    printf("Pass threshold:          %.1f\n", PASS_THRESHOLD);
    printf("Students passing:        %d (%.1f%%)\n",
           pass_count, (float)pass_count / db->count * 100);
    printf("Students below threshold:%d (%.1f%%)\n",
           fail_count, (float)fail_count / db->count * 100);
    printf("========================================================\n");

    // Include risk analysis results in summary
    printf("\n--- Academic Risk Index Results ---\n");
    int high_risk = 0;
    ptr = db->records;
    for (i = 0; i < db->count; i++, ptr++) {
        float risk = calculate_risk_score(ptr->grade, average);
        if (risk >= RISK_HIGH_THRESHOLD) {
            high_risk++;
        }
    }
    printf("Students at HIGH academic risk: %d\n", high_risk);
    if (high_risk > 0) {
        printf("These students need immediate intervention.\n");
    } else {
        printf("No students currently at high academic risk.\n");
    }
    printf("========================================================\n");
}

// ---------------------------------------------------------------
// Initial data loading
// ---------------------------------------------------------------

// Loads the initial dataset of 12 students into the database
void load_initial_data(StudentDB *db) {
    Student initial_data[] = {
        {1001, "Amara Okafor",     "Data Structures",    78.5},
        {1002, "Brian Kimani",     "Algorithms",         92.0},
        {1003, "Chioma Eze",       "Operating Systems",  45.0},
        {1004, "David Mensah",     "Data Structures",    63.5},
        {1005, "Esther Wanjiku",   "Algorithms",         88.0},
        {1006, "Felix Adeyemi",    "Computer Networks",  35.0},
        {1007, "Grace Achieng",    "Operating Systems",  71.0},
        {1008, "Hassan Diallo",    "Data Structures",    55.5},
        {1009, "Irene Banda",      "Computer Networks",  82.0},
        {1010, "James Mwangi",     "Algorithms",         29.5},
        {1011, "Kemi Ogundimu",    "Operating Systems",  67.0},
        {1012, "Liam Nkrumah",     "Data Structures",    91.5}
    };

    int num_students = sizeof(initial_data) / sizeof(initial_data[0]);
    int i;

    for (i = 0; i < num_students; i++) {
        if (db->count >= db->capacity) {
            int new_cap = db->capacity * 2;
            Student *new_rec = (Student *)realloc(db->records, new_cap * sizeof(Student));
            if (new_rec == NULL) {
                printf("Error: Memory allocation failed during data loading.\n");
                return;
            }
            db->records = new_rec;
            db->capacity = new_cap;
        }

        Student *dest = db->records + db->count;
        *dest = initial_data[i];
        db->count++;
    }

    printf("Loaded %d initial student records.\n", num_students);
}

// ---------------------------------------------------------------
// Main function
// ---------------------------------------------------------------

