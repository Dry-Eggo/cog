#include <stdio.h>
#include <juve_utils.h>
#include <stdlib.h>

juve_arena_t* global_arena;

extern void print_usage(const char* program_name) {
    fprintf(stdout, "usage: %s [option...] [arg...]\n", program_name);
    fprintf(stdout, "option:\n");
    fprintf(stdout, "    %5s <path>    Input path\n", "-i, --source");
    fprintf(stdout, "    %5s <path>    Output path\n", "-o, --output");
    fprintf(stdout, "    %5s          Verbose logging\n", "-b, --verbose");
}

int main(int argc, char** argv) {
    global_arena = jarena_new();
    jcli_args_t* args = jcli_new_a(global_arena);
    jcli_parse(argc, argv, args, global_arena);

    bool verbose = false;
    char* program_name = argv[0];
    char* output_file = NULL;
    char* input_file = NULL;
    
    if (jcli_has_flag(args, "verbose")) {
	verbose = true;
    }
    
    if (!jcli_get_option_sl(args, "o", "output", &output_file)) {
	// TODO: stem the input and add extension '.o'
    }

    if (!jcli_get_option_sl(args, "i", "source", &input_file)) {
	// TODO: get from positionals
	print_usage(program_name);
	printf("Kudo: No input was provided\n");
    }
    
    jarena_free(global_arena);
    return 0;
}
