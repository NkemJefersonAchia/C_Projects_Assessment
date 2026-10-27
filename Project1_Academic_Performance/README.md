# Project 1: Dynamic Academic Performance and Risk Analysis Engine

A menu-driven student record management system that evaluates academic performance, identifies at-risk students, and generates statistical summaries using dynamic memory allocation, structures, function pointers, and pointer-based traversal.

## Compilation

```bash
gcc -Wall -Wextra -std=c99 -o academic_engine academic_engine.c -lm
./academic_engine
```

## Features

- Add, search, update, and remove student records dynamically
- Sort students by grade (ascending/descending) using pointer-based traversal
- Generate summary statistics (mean, min, max, standard deviation)
- Custom analysis: Academic Risk Index with weighted risk scoring
- Menu-driven interface dispatched via function pointer array
