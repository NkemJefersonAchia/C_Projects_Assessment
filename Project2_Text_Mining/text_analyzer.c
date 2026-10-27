// text_analyzer.c - Dynamic Text Mining and Linguistic Pattern Analyzer
// Processes a paragraph from a published text, identifies linguistic patterns,
// calculates word statistics, and generates a text summary.
// Uses structures, dynamic memory allocation, pointers, and function pointers.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ---------------------------------------------------------------
// Constants
// ---------------------------------------------------------------
#define MAX_WORD_LEN 40
#define INITIAL_TEXT_SIZE 2048
#define INITIAL_WORD_CAPACITY 100
#define NUM_ANALYSIS_OPS 9

// ---------------------------------------------------------------
// Structures
// ---------------------------------------------------------------

// Word statistics structure - stores each distinct word and its frequency
typedef struct {
    char name[MAX_WORD_LEN];
    int count;
} WordStat;

// Text data container for the entire analysis session
typedef struct {
    char *text;
    int text_length;
    int text_capacity;
    WordStat *words;
    int word_count;
    int word_capacity;
    int total_words;
} TextData;

// Function pointer type for analysis operations
typedef void (*AnalysisFunction)(TextData *);
