#include <stdio.h>
#include <driver.h>
#include <cli/cli.h>


juve_arena_t* global_arena;
color_manager_t global_color_manager;

extern void print_usage(const char* program_name) {
    fprintf(stdout, "usage: %s [option...] [arg...]\n", program_name);
    fprintf(stdout, "option:\n");
    fprintf(stdout, "    -i, --source <path>    Input path\n");
    fprintf(stdout, "    -o, --output <path>    Output path\n");
    fprintf(stdout, "    -b, --verbose          Verbose logging\n");
    fprintf(stdout, "    -t, --test             Test mode. exits with success even on error\n");
}

int main(int argc, char** argv) {
    global_arena = jarena_new();
    cm_init(&global_color_manager);

    compile_options_t compile_options = {0};    
    parse_args(&compile_options, argc, argv);

    kudo_compile(&compile_options);
    
    jarena_free(global_arena);
    return 0;
}
