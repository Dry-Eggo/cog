#include <stdbool.h>
#include <cli/cli.h>
#include <compiler_opt.h>
#include <string.h>
#include <stdio.h>
#include <defines.h>

extern void print_usage(const char* prog);

char* shift(int* c, char** v) { return v[(*c)++]; }

void parse_short_flag(CompileOptions* opts, char flag, int* i, int argc, char** argv) {
    switch (flag) {
    case 'i':
        if (*i < argc) {
            opts->input_file = shift(i, argv);
        } else {
            print_usage(argv[0]);
            LOG_ERR("'-%c' expected an argument\n", flag);
            exit(1);
        }
        break;
    case 'o':
        if (*i < argc) {
            opts->output_file = shift(i, argv);
        } else {
            print_usage(argv[0]);
            LOG_ERR("'-%c' expected an argument\n", flag);
            exit(1);
        }
        break;
    case 'b':
        opts->verbose_logging = true;
        break;
    case 'h':
        print_usage(argv[0]);
        exit(0);        
        break;
    case 't':
        opts->test_mode = true;        
        break;
    case 'D':
        TODO("compiler defines not implemented yet: '%s'", argv[*i - 1] + 2);
        break;
    default:
        LOG_ERR("unknown flag '-%c'\n", flag);
        exit(1);
        break;
    }
}

void parse_short_flags(CompileOptions* opts, int* i, int argc, char** argv) {
    const char* arg = argv[*i - 1] + 1;
    size_t len = strlen(arg);
    if (len > 1) {
        int cursor = *i;
        for (size_t n = 0; n < len; ++n) {
            char flag = arg[n];
            parse_short_flag(opts, flag, i, argc, argv);
            
            // was an argument taken?
            if (cursor != *i && n != len - 1) {
                LOG_ERR("in '-%s', '%c' takes an argument. consider seperating it\n", arg, flag);
                exit(1);
            }
        }
    } else {
        parse_short_flag(opts, *arg, i, argc, argv);
    }
}

void parse_long_flags(CompileOptions* opts, int* i, int argc, char** argv) {
    char* arg = argv[*i - 1] + 2;
    size_t len = strlen(arg);

    if (strncmp(arg, "output", 6) == 0) {
        if (len >= 6 && arg[6] == '=') {
            const char* value = arg + 7;
            if (strlen(value) == 0) {
                print_usage(argv[0]);
                LOG_ERR("'--output' expected an argument\n");
                exit(1);                
            } else {
                opts->output_file = shift(i, argv);                
                return;
            }
        }
        if (*i < argc) {
            opts->output_file = shift(i, argv);
        } else {
            print_usage(argv[0]);
            LOG_ERR("'--output' expected an argument\n");
            exit(1);
        }
    }

    else if (strncmp(arg, "input", 5) == 0) {
        if (len >= 5 && arg[5] == '=') {
            const char* value = arg + 6;
            if (strlen(value) == 0) {
                print_usage(argv[0]);
                LOG_ERR("'--input' expected an argument\n");
                exit(1);                
            } else {
                opts->input_file = shift(i, argv);                
                return;
            }
        }
        if (*i < argc) {
            opts->input_file = shift(i, argv);
        } else {
            print_usage(argv[0]);
            LOG_ERR("'--input' expected an argument\n");
            exit(1);
        }
    }

    else if (strcmp(arg, "verbose") == 0) {
        opts->verbose_logging = true;
    }
    else if (strcmp(arg, "emit-c") == 0) {
        opts->emit_c = true;
    }
    else if (strcmp(arg, "test") == 0) {
        opts->test_mode = true;
    }
    else if (strcmp(arg, "help") == 0) {
        print_usage(argv[0]);
        exit(1);
    }
    else {
        LOG_ERR("unknown argument: '%s'. skipping\n", arg);
    }
}

void parse_args(CompileOptions* opts, int argc, char** argv) {
    int i = 0;
    opts->program_name = shift(&i, argv);

    while (i < argc) {
        char* arg = shift(&i, argv);
        if (strlen(arg) >= 2 && (arg[0] == '-' && arg[1] != '-')) {
            parse_short_flags(opts, &i, argc, argv);
        } else if (strlen(arg) >= 2 && strncmp(arg, "--", 2) == 0) {
            parse_long_flags(opts, &i, argc, argv);
        } else if (arg[0] != '-') {
            if (opts->input_file) {
                fprintf(stderr, "Kudo Note: '%s' was ignored\n", arg);
                continue;
            } else {
                opts->input_file = arg;
                continue;
            }            
        } else {
            LOG_ERR("unknown argument: '%s'. skipping\n", arg);
        }
    }
    
}
