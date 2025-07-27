#include <driver.h>
#include <errors.h>
#include <defines.h>
#include <stdio.h>
#include <stdlib.h>
#include <semantics.h>

void driver_free(driver_t* driver);

void abort_compilation(driver_t* driver) {
    int exit_code = 1;
    if (driver) {
        exit_code = driver->options->test_mode ? 0 : 1;
        driver_free(driver);
    }
    
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

void driver_free(driver_t* driver) {    
    jvec_free(driver->source_lines);
    if (driver->phase >= phase_sema_k) sema_free(driver->sema);
}

void kudo_compile(compile_options_t* compile_options) {
    if (!jfile_exists(compile_options->input_file)) {
	    log_err("Unable to open file: '%s': file not found\n", compile_options->input_file);
        abort_compilation(NULL);
    }
    
    juve_buffer_t* buffer = jb_create();
    jb_read_entire_file(compile_options->input_file, buffer);

    const char* source = jb_str_a(buffer, global_arena);
    
    driver_t* driver = driver_new(source, compile_options);
    
    driver->lexer = lexer_new(compile_options, source);
    lexer_lex(driver->lexer);

    if (compile_options->verbose_logging) {
	    printf("========================\n");
	    int max = cjvec_len(driver->lexer->tokens);
	    for (int i = 0; i < max; ++i) {
	        token_t tok = *(token_t*)cjvec_at(driver->lexer->tokens, i);
	        printf("|| Token(%d : '%s')\n", (int)tok.kind, tok.text);
	    }
	    printf("========================\n");
    }

    driver->phase = phase_parser_k;
    driver->parser = parser_new(compile_options, driver->lexer->tokens, driver->lexer->source);
    if (!parser_parse(driver->parser)) {
	    syntax_error_flush(driver->parser->errors, driver->source_lines);
	    fprintf(stderr, "Kudo: %ld parsing errors occured\n", cjvec_len(driver->parser->errors));
	    jb_free(buffer);
	    abort_compilation(driver);
    }

    driver->phase = phase_sema_k;
    driver->sema = semantics_init(driver->parser->items, driver->source_lines, driver->lexer->source, compile_options);
    if (!sema_check(driver->sema)) {
        sema_error_flush(sema_get_diagnostics(driver->sema), driver->source_lines);
        abort_compilation(driver);
    }

    driver->phase = phase_codegen_k;
    driver->c_ctx = c_ctx_new(driver->parser->items, compile_options);
    if (!c_ctx_emit(driver->c_ctx)) {
        
    }
    
    jb_print(sema_get_tmp(driver->sema));    
    jb_free(buffer);
    driver_free(driver);
}
