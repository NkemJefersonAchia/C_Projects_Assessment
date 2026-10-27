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
