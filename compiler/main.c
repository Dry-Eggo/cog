#include <stdio.h>
#include <driver.h>


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
    jcli_args_t* args = jcli_new_a(global_arena);
    jcli_parse(argc, argv, args, global_arena);

    compile_options_t compile_options = {0};
    
    compile_options.program_name = argv[0];
    
    if (jcli_has_flag_sl(args, "b", "verbose")) {
	compile_options.verbose_logging = true;
    } else {
	compile_options.verbose_logging = false;
    }
    
    if (jcli_has_flag_sl(args, "t", "test")) {
	compile_options.test_mode = true;
    } else {
	compile_options.test_mode = false;
    }
    
    if (!jcli_get_option_sl(args, "o", "output", &compile_options.output_file)) {
	// TODO: stem the input and add extension '.o'
    }

    if (!jcli_get_option_sl(args, "i", "source", &compile_options.input_file)) {
	// TODO: get from positionals
	print_usage(compile_options.program_name);
	printf("Kudo: No input was provided\n");
    }

    kudo_compile(&compile_options, args);
    
    jarena_free(global_arena);
    return 0;
}
