#include <driver.h>
#include <errors.h>
#include <defines.h>
#include <stdio.h>
#include <stdlib.h>
#include <semantics.h>

void abort_compilation(driver_t* driver) {
    int exit_code = driver->options->test_mode ? 0 : 1;
    jvec_free(driver->lexer->tokens);
    
    jvec_free(driver->parser->errors);
    jvec_free(driver->parser->items);
    
    jvec_free(driver->source_lines);
    jarena_free(global_arena);
    fprintf(stderr, "Kudo: compilation aborted\n");
    exit(exit_code);
}

driver_t* driver_new(const char* source, compile_options_t* opts) {
    driver_t* driver = (driver_t*)jarena_alloc(global_arena, sizeof(driver_t));
    driver->options = opts;
    driver->phase = phase_lexer_k;
    driver->source = source;
    driver->source_lines = jvec_lines(source, global_arena);
    return driver;
}

void kudo_compile(compile_options_t* compile_options, jcli_args_t* args) {
    if (!jfile_exists(compile_options->input_file)) {
	    log_err("Unable to open file: '%s': file not found\n", compile_options->input_file);
	    jcli_args_free(args);
	    exit(1);
    }
    
    juve_buffer_t* buffer = jb_create();
    jb_read_entire_file(compile_options->input_file, buffer);

    const char* source = jb_str_a(buffer, global_arena);
    
    driver_t* driver = driver_new(source, compile_options);
    
    driver->lexer = lexer_new(compile_options, source);
    lexer_lex(driver->lexer);

    if (compile_options->verbose_logging) {
	    printf("========================\n");
	    int max = jvec_len(driver->lexer->tokens);
	    for (int i = 0; i < max; ++i) {
	        token_t tok = *(token_t*)jvec_at(driver->lexer->tokens, i);
	        printf("|| Token(%d : '%s')\n", (int)tok.kind, tok.text);
	    }
	    printf("========================\n");
    }
    
    driver->parser = parser_new(compile_options, driver->lexer->tokens, driver->lexer->source);
    if (!parser_parse(driver->parser)) {
	    syntax_error_flush(driver->parser->errors, driver->source_lines);
	    fprintf(stderr, "Kudo: %ld parsing errors occured\n", jvec_len(driver->parser->errors));
	    jb_free(buffer);
	    jcli_args_free(args);
	    abort_compilation(driver);
    }

    printf("Total top level items: %ld\n", jvec_len(driver->parser->items));

    driver->sema = semantics_init(driver->parser->items, driver->source_lines, driver->lexer->source, compile_options);
    sema_check(driver->sema);

    jb_print(sema_get_tmp(driver->sema));
    
    jb_free(buffer);
}
