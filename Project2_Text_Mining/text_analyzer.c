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

// ---------------------------------------------------------------
// Lifecycle functions
// ---------------------------------------------------------------

// Creates and initializes a TextData container with dynamic memory
TextData *create_text_data(void) {
    TextData *data = (TextData *)malloc(sizeof(TextData));
    if (data == NULL) {
        printf("Error: Failed to allocate TextData structure.\n");
        return NULL;
    }

    data->text = (char *)malloc(INITIAL_TEXT_SIZE * sizeof(char));
    if (data->text == NULL) {
        printf("Error: Failed to allocate text buffer.\n");
        free(data);
        return NULL;
    }

    data->words = (WordStat *)calloc(INITIAL_WORD_CAPACITY, sizeof(WordStat));
    if (data->words == NULL) {
        printf("Error: Failed to allocate word statistics array.\n");
        free(data->text);
        free(data);
        return NULL;
    }

    data->text[0] = '\0';
    data->text_length = 0;
    data->text_capacity = INITIAL_TEXT_SIZE;
    data->word_count = 0;
    data->word_capacity = INITIAL_WORD_CAPACITY;
    data->total_words = 0;

    return data;
}

// Frees all dynamically allocated memory
void destroy_text_data(TextData *data) {
    if (data != NULL) {
        if (data->text != NULL) free(data->text);
        if (data->words != NULL) free(data->words);
        free(data);
    }
}

// ---------------------------------------------------------------
// Text processing helpers
// ---------------------------------------------------------------

// Converts a string to lowercase in place using pointer traversal
void to_lowercase(char *str) {
    char *ptr = str;
    while (*ptr != '\0') {
        *ptr = tolower((unsigned char)*ptr);
        ptr++;
    }
}

// Checks if a character is part of a word
// Letters and apostrophes are word characters; hyphens are separators
int is_word_char(char c) {
    return isalpha((unsigned char)c) || c == '\'';
}

// Adds a word to the word statistics array, incrementing count if duplicate
void add_word(TextData *data, const char *word) {
    // Search for existing word using pointer traversal and strcmp
    WordStat *ptr = data->words;
    int i;
    for (i = 0; i < data->word_count; i++, ptr++) {
        if (strcmp(ptr->name, word) == 0) {
            ptr->count++;
            return;
        }
    }

    // Word not found - expand array if needed
    if (data->word_count >= data->word_capacity) {
        int new_capacity = data->word_capacity * 2;
        WordStat *new_words = (WordStat *)realloc(data->words,
                                                    new_capacity * sizeof(WordStat));
        if (new_words == NULL) {
            printf("Error: Failed to expand word statistics array.\n");
            return;
        }
        data->words = new_words;
        data->word_capacity = new_capacity;
    }

    // Copy word into the new entry safely using strncpy
    WordStat *new_entry = data->words + data->word_count;
    strncpy(new_entry->name, word, MAX_WORD_LEN - 1);
    new_entry->name[MAX_WORD_LEN - 1] = '\0';
    new_entry->count = 1;
    data->word_count++;
}

// ---------------------------------------------------------------
// Text input functions
// ---------------------------------------------------------------

// Loads the default paragraph from "Things Fall Apart" by Chinua Achebe
