#include <stdlib.h>
#include <driver.h>
#include <cli/cli.h>


JArena* global_arena;
ColorManager global_color_manager;

extern void print_usage(const char* program_name) {
    fprintf(stdout, "usage: %s [option...] [arg...]\n", program_name);
    fprintf(stdout, "option:\n");
    fprintf(stdout, "    -i, --source <path>    Input path\n");
    fprintf(stdout, "    -o, --output <path>    Output path\n");
    fprintf(stdout, "    -b, --verbose          Verbose logging\n");
    fprintf(stdout, "    -t, --test             Test mode. exits with success even on error\n");
    fprintf(stdout, "    --emit-c               Emit the c file used to produce the exectuable\n\n");
}

int main(int argc, char** argv) {
    JArena arena = jarena_new();
    global_arena = &arena;
    
    cm_init(&global_color_manager);
    
    CompileOptions compile_options = {0};    
    parse_args(&compile_options, argc, argv);

    cog_compile(&compile_options);
    
    jarena_free(global_arena);
    return 0;
}
