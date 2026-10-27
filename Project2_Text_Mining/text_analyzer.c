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
