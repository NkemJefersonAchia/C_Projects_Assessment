# Project 2: Dynamic Text Mining and Linguistic Pattern Analyzer

A text analysis engine that processes paragraphs, extracts word statistics, and performs linguistic analysis using pointer-based text parsing, dynamic memory allocation, and function pointers.

## Compilation

```bash
gcc -Wall -Wextra -std=c99 -o text_analyzer text_analyzer.c
./text_analyzer
```

## Features

- Load default text or enter custom paragraphs for analysis
- Extract and count words using pointer-based traversal
- Find most/least frequent words and display word frequencies
- Inspect memory addresses of dynamically allocated structures
- Custom analysis: Lexical Diversity Score measuring vocabulary richness
- Menu-driven interface dispatched via function pointer array
