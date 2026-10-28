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
void load_default_text(TextData *data) {
    const char *default_text =
        "Okonkwo was well known throughout the nine villages and even beyond. "
        "His fame rested on solid personal achievements. As a young man of eighteen "
        "he had brought honor to his village by throwing Amalinze the Cat. Amalinze "
        "was the great wrestler who for seven years was unbeaten from Umuofia to "
        "Mbaino. He was called the Cat because his back would never touch the earth. "
        "It was this man that Okonkwo threw in a fight which the old men agreed was "
        "one of the fiercest since the founder of their town engaged a spirit of the "
        "wild for seven days. The drums beat and the flutes sang and the spectators "
        "held their breath. Amalinze was a wily craftsman but Okonkwo was as slippery "
        "as a fish in water. Every nerve and every muscle stood out on their arms on "
        "their backs and their thighs and one almost heard them stretching to breaking "
        "point. In the end Okonkwo threw the Cat.";

    int len = strlen(default_text);

    // Ensure buffer is large enough
    if (len + 1 > data->text_capacity) {
        char *new_buf = (char *)realloc(data->text, (len + 1) * sizeof(char));
        if (new_buf == NULL) {
            printf("Error: Failed to allocate memory for text.\n");
            return;
        }
        data->text = new_buf;
        data->text_capacity = len + 1;
    }

    strcpy(data->text, default_text);
    data->text_length = len;
    data->word_count = 0;
    data->total_words = 0;

    printf("Default paragraph loaded (%d characters).\n", len);
    printf("Source: \"Things Fall Apart\" by Chinua Achebe (Chapter 1)\n");
}

// Reads text input from the user, dynamically expanding the buffer
void input_text(TextData *data) {
    printf("\nPaste or type your paragraph (press Enter twice to finish):\n");
    printf("-----------------------------------------------------------\n");

    data->text[0] = '\0';
    data->text_length = 0;
    data->word_count = 0;
    data->total_words = 0;

    int consecutive_newlines = 0;
    int c;

    while ((c = getchar()) != EOF) {
        if (c == '\n') {
            consecutive_newlines++;
            if (consecutive_newlines >= 2) break;
        } else {
            consecutive_newlines = 0;
        }

        // Expand buffer if needed
        if (data->text_length + 2 >= data->text_capacity) {
            int new_capacity = data->text_capacity * 2;
            char *new_text = (char *)realloc(data->text, new_capacity * sizeof(char));
            if (new_text == NULL) {
                printf("Error: Failed to expand text buffer.\n");
                break;
            }
            data->text = new_text;
            data->text_capacity = new_capacity;
        }

        data->text[data->text_length] = (char)c;
        data->text_length++;
        data->text[data->text_length] = '\0';
    }

    if (data->text_length > 0) {
        printf("Text loaded successfully (%d characters).\n", data->text_length);
    } else {
        printf("Warning: No text was entered.\n");
    }
}

// Wrapper for loading text - gives option to use default or custom input
void load_text_wrapper(TextData *data) {
    int choice;
    printf("\n--- Load Text ---\n");
    printf("1. Use default paragraph (Things Fall Apart)\n");
    printf("2. Enter custom paragraph\n");
    printf("Enter choice: ");

    if (scanf("%d", &choice) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (choice == 1) {
        load_default_text(data);
    } else if (choice == 2) {
        input_text(data);
    } else {
        printf("Invalid choice.\n");
    }
}

// ---------------------------------------------------------------
// Text processing
// ---------------------------------------------------------------

// Processes the loaded text into word statistics using pointer-based traversal
// Punctuation: strips non-letter/non-apostrophe characters
// Case: all words converted to lowercase
// Hyphens: treated as word separators
// Long words: truncated to 39 characters to fit buffer
void process_text(TextData *data) {
    if (data->text_length == 0) {
        printf("No text to process. Please load text first.\n");
        return;
    }

    printf("\n--- Text Processing Rules ---\n");
    printf("  - Letters (a-z, A-Z): included in words\n");
    printf("  - Apostrophes: kept within words (e.g., \"don't\")\n");
    printf("  - Hyphens: treated as word separators\n");
    printf("  - Other punctuation: stripped (periods, commas, etc.)\n");
    printf("  - Case: all words converted to lowercase\n");
    printf("  - Long words: truncated to 39 characters\n");

    data->word_count = 0;
    data->total_words = 0;

    char word_buffer[MAX_WORD_LEN];
    int word_len = 0;

    // Pointer-based traversal of the text buffer
    char *ptr = data->text;
    char *end = data->text + data->text_length;

    while (ptr <= end) {
        if (ptr < end && is_word_char(*ptr)) {
            if (word_len < MAX_WORD_LEN - 1) {
                word_buffer[word_len] = *ptr;
                word_len++;
            }
        } else {
            if (word_len > 0) {
                word_buffer[word_len] = '\0';

                // Strip trailing apostrophes
                while (word_len > 0 && word_buffer[word_len - 1] == '\'') {
                    word_buffer[word_len - 1] = '\0';
                    word_len--;
                }
                // Strip leading apostrophes
                char *start = word_buffer;
                while (*start == '\'') start++;

                if (strlen(start) > 0) {
                    to_lowercase(start);
                    add_word(data, start);
                    data->total_words++;
                }

                word_len = 0;
            }
        }
        ptr++;
    }

    printf("Text processed: %d total words, %d distinct words.\n",
           data->total_words, data->word_count);
}

// ---------------------------------------------------------------
// Analysis operations
// ---------------------------------------------------------------

// Displays the total word count
void count_words(TextData *data) {
    if (data->total_words == 0) {
        printf("\nNo text has been processed yet.\n");
        return;
    }
    printf("\n--- Word Count ---\n");
    printf("Total words:    %d\n", data->total_words);
    printf("Distinct words: %d\n", data->word_count);
}

// Finds and displays the longest word using pointer traversal and strlen
void longest_word(TextData *data) {
    if (data->word_count == 0) {
        printf("\nNo words available. Process text first.\n");
        return;
    }

    WordStat *longest = data->words;
    int max_len = strlen(longest->name);

    WordStat *ptr = data->words + 1;
    WordStat *end = data->words + data->word_count;
    while (ptr < end) {
        int len = strlen(ptr->name);
        if (len > max_len) {
            max_len = len;
            longest = ptr;
        }
        ptr++;
    }

    printf("\n--- Longest Word ---\n");
    printf("Word:   \"%s\"\n", longest->name);
    printf("Length: %d characters\n", max_len);
}

// Finds and displays the most frequently occurring word
void most_frequent(TextData *data) {
    if (data->word_count == 0) {
        printf("\nNo words available. Process text first.\n");
        return;
    }

    WordStat *most_freq = data->words;
    WordStat *ptr = data->words + 1;
    WordStat *end = data->words + data->word_count;
    while (ptr < end) {
        if (ptr->count > most_freq->count) {
            most_freq = ptr;
        }
        ptr++;
    }

    printf("\n--- Most Frequent Word ---\n");
    printf("Word:      \"%s\"\n", most_freq->name);
    printf("Frequency: %d times\n", most_freq->count);
}

// Displays all distinct words and their frequencies
void display_word_statistics(TextData *data) {
    if (data->word_count == 0) {
        printf("\nNo words available. Process text first.\n");
        return;
    }

    printf("\n%-5s %-30s %-10s\n", "#", "Word", "Frequency");
    printf("----------------------------------------------\n");

    WordStat *ptr = data->words;
    int i;
    for (i = 0; i < data->word_count; i++, ptr++) {
        printf("%-5d %-30s %-10d\n", i + 1, ptr->name, ptr->count);
    }
    printf("----------------------------------------------\n");
    printf("Total distinct words: %d\n", data->word_count);
}

// Sorts word statistics alphabetically or by frequency
void sort_word_statistics(TextData *data) {
    if (data->word_count < 2) {
        printf("\nNot enough words to sort.\n");
        return;
    }

    int choice;
    printf("\n--- Sort Word Statistics ---\n");
    printf("1. Sort alphabetically\n");
    printf("2. Sort by frequency (descending)\n");
    printf("Enter choice: ");

    if (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2)) {
        printf("Invalid sort option.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    // Bubble sort using pointer-based traversal
    int i, j;
    for (i = 0; i < data->word_count - 1; i++) {
        for (j = 0; j < data->word_count - i - 1; j++) {
            WordStat *current = data->words + j;
            WordStat *next = data->words + j + 1;
            int should_swap = 0;

            if (choice == 1) {
                should_swap = (strcmp(current->name, next->name) > 0);
            } else {
                should_swap = (current->count < next->count);
            }

            if (should_swap) {
                WordStat temp = *current;
                *current = *next;
                *next = temp;
            }
        }
    }

    printf("Words sorted %s.\n",
           choice == 1 ? "alphabetically" : "by frequency (descending)");
    display_word_statistics(data);
}

// Searches for a specific word and reports its frequency
void search_word(TextData *data) {
    if (data->word_count == 0) {
        printf("\nNo words available. Process text first.\n");
        return;
    }

    char search_term[MAX_WORD_LEN];
    printf("\nEnter word to search: ");
    if (fgets(search_term, MAX_WORD_LEN, stdin) == NULL) {
        printf("Error reading input.\n");
        return;
    }
    search_term[strcspn(search_term, "\n")] = '\0';
    to_lowercase(search_term);

    WordStat *ptr = data->words;
    int i;
    for (i = 0; i < data->word_count; i++, ptr++) {
        if (strcmp(ptr->name, search_term) == 0) {
            printf("Word \"%s\" found with frequency: %d\n",
                   ptr->name, ptr->count);
            return;
        }
    }
    printf("Word \"%s\" not found in the text.\n", search_term);
}

// ---------------------------------------------------------------
// Custom Analysis: Lexical Diversity Score
// ---------------------------------------------------------------

// Measures the richness of vocabulary in the text
// Formula: LDS = distinct_words / total_words (ranges 0 to 1)
// Levels: Very High (0.80+), High (0.60+), Moderate (0.40+),
//         Low (0.20+), Very Low (below 0.20)
void lexical_diversity_analysis(TextData *data) {
    if (data->word_count == 0 || data->total_words == 0) {
        printf("\nNo text has been processed for analysis.\n");
        return;
    }

    float lds = (float)data->word_count / data->total_words;

    const char *level;
    if (lds >= 0.80) level = "Very High";
    else if (lds >= 0.60) level = "High";
    else if (lds >= 0.40) level = "Moderate";
    else if (lds >= 0.20) level = "Low";
    else level = "Very Low";

    // Count repeated words and hapax legomena (words appearing once)
    int repeated_words = 0;
    int hapax_count = 0;
    WordStat *most_repeated = data->words;

    WordStat *ptr = data->words;
    int i;
    for (i = 0; i < data->word_count; i++, ptr++) {
        if (ptr->count > 1) repeated_words++;
        if (ptr->count == 1) hapax_count++;
        if (ptr->count > most_repeated->count) most_repeated = ptr;
    }

    float repetition_intensity = (float)repeated_words / data->word_count;
    float avg_frequency = (float)data->total_words / data->word_count;

    printf("\n========================================================\n");
    printf("       LEXICAL DIVERSITY SCORE ANALYSIS\n");
    printf("========================================================\n");
    printf("Formula: LDS = Distinct Words / Total Words\n");
    printf("--------------------------------------------------------\n");
    printf("Total words:             %d\n", data->total_words);
    printf("Distinct words:          %d\n", data->word_count);
    printf("Lexical Diversity Score: %.4f (%.1f%%)\n", lds, lds * 100);
    printf("Diversity Level:         %s\n", level);
    printf("--------------------------------------------------------\n");
    printf("Additional Metrics:\n");
    printf("  Words appearing once (hapax): %d (%.1f%% of distinct)\n",
           hapax_count, (float)hapax_count / data->word_count * 100);
    printf("  Repeated words:               %d (%.1f%% of distinct)\n",
           repeated_words, repetition_intensity * 100);
    printf("  Average word frequency:        %.2f\n", avg_frequency);
    printf("  Most repeated word:            \"%s\" (%d times)\n",
           most_repeated->name, most_repeated->count);
    printf("========================================================\n");
    printf("Interpretation:\n");
    if (lds >= 0.60) {
        printf("  The text has a rich vocabulary with good word variety.\n");
    } else if (lds >= 0.40) {
        printf("  The text has moderate diversity - a mix of unique and\n");
        printf("  repeated words, typical of natural prose.\n");
    } else {
        printf("  The text is repetitive, with many frequently reused words.\n");
    }
    printf("========================================================\n");
}

// ---------------------------------------------------------------
// Memory Address Inspection
// ---------------------------------------------------------------

// Prints memory addresses of key data structures using %p and (void *) casts
