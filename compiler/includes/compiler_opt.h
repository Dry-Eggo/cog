#pragma once

typedef struct  {
    bool verbose_logging;
    bool test_mode;
    bool emit_c;
    bool set_env_mode;

    char* runtime_header_path;
    char* output_file;
    char* input_file;
    char* program_name;
} CompileOptions;
