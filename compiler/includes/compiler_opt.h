#pragma once

typedef struct  {
    bool verbose_logging;
    bool test_mode;
    char* output_file;
    char* input_file;
    char* program_name;
} CompileOptions;
