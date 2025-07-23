#include <stdio.h>
#include <juve_utils.h>
#include <stdlib.h>
#include <lexer.h>
#include <compiler_opt.h>

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

    compile_options_t compile_options = {0};
    
    compile_options.program_name = argv[0];
    
    if (jcli_has_flag_sl(args, "b", "verbose")) {
	compile_options.verbose_logging = true;
    }
    
    if (!jcli_get_option_sl(args, "o", "output", &compile_options.output_file)) {
	// TODO: stem the input and add extension '.o'
    }

    if (!jcli_get_option_sl(args, "i", "source", &compile_options.input_file)) {
	// TODO: get from positionals
	print_usage(compile_options.program_name);
	printf("Kudo: No input was provided\n");
    }

    lexer_t* lexer = lexer_new(&compile_options);
    lexer_lex(lexer);

    if (compile_options.verbose_logging) {
	printf("========================\n");
	int max = jvec_len(lexer->tokens);
	for (int i = 0; i < max; ++i) {
	    token_t tok = *(token_t*)jvec_at(lexer->tokens, i);
	    printf("|| Token(%d : '%s')\n", (int)tok.kind, tok.text, ' ');
	}
	printf("========================\n");
    }
    
    jarena_free(global_arena);
    return 0;
}
