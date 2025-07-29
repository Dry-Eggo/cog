#include <driver.h>
#include <errors.h>
#include <defines.h>
#include <stdio.h>
#include <stdlib.h>
#include <semantics.h>

void driver_free(Driver* driver);

void abort_compilation(Driver* driver) {
    int exit_code = 1;
    if (driver) {
        exit_code = driver->options->test_mode ? 0 : 1;
        driver_free(driver);
    }
    
    jarena_free(global_arena);
    fprintf(stderr, "Kudo: compilation aborted\n");
    exit(exit_code);
}

Driver* driver_new(const char* source, CompileOptions* opts) {
    Driver* driver = (Driver*)jarena_alloc(global_arena, sizeof(Driver));
    driver->options = opts;
    driver->phase = phase_lexer_k;
    driver->source = source;
    driver->source_lines = jvec_lines(source, global_arena);
    return driver;
}

void driver_free(Driver* driver) {    
    jvec_free(driver->source_lines);
    jb_free(driver->source_buffer);
    if (driver->phase >= phase_sema_k) sema_free(driver->sema);
}

void emit_file(CompileOptions* compile_options, JBuffer* compiled_file) {
    const char* filestem = jfile_stem(compile_options->input_file, global_arena);
    CJBuffer* tmp = cjb_create(global_arena);
    
    cjb_appendf(tmp, "%s.c", filestem);
    const char* tmp_cfile = cjb_str(tmp);
    cjb_clear(tmp);

    const char* content = jb_str_a(compiled_file, global_arena);
    
    jfile_write(tmp_cfile, content);

    CJCmd cmd = jcmd_init(global_arena, JCMD_NOT_SET);
    
    if (!compile_options->output_file) {       
        cjb_appendf(tmp, "%s.o", filestem);
        compile_options->output_file = cjb_str(tmp);
    }
    
    cmd_append(&cmd, "clang", "-c -o", compile_options->output_file, tmp_cfile);
    jcmd_one_shot(&cmd);

    if (compile_options->test_mode) printf("%s", content);
}

void kudo_compile(CompileOptions* compile_options) {
    if (!jfile_exists(compile_options->input_file)) {
	    LOG_ERR("Unable to open file: '%s': file not found\n", compile_options->input_file);
        abort_compilation(NULL);
    }
    
    JBuffer* buffer = jb_create();
    jb_read_entire_file(compile_options->input_file, buffer);

    const char* source = jb_str_a(buffer, global_arena);
    
    Driver* driver = driver_new(source, compile_options);
    driver->source_buffer = buffer;
    
    driver->lexer = lexer_new(compile_options, source);
    lexer_lex(driver->lexer);

    if (compile_options->verbose_logging) {
	    printf("========================\n");
	    int max = cjvec_len(driver->lexer->tokens);
	    for (int i = 0; i < max; ++i) {
	        Token tok = *(Token*)cjvec_at(driver->lexer->tokens, i);
	        printf("|| Token(%d : '%s')\n", (int)tok.kind, tok.text);
	    }
	    printf("========================\n");
    }

    driver->phase = phase_parser_k;
    driver->parser = parser_new(compile_options, driver->lexer->tokens, driver->lexer->source);
    if (!parser_parse(driver->parser)) {
	    syntax_error_flush(driver->parser->errors, driver->source_lines);
	    fprintf(stderr, "Kudo: %ld parsing errors occurred\n", cjvec_len(driver->parser->errors));
	    abort_compilation(driver);
    }

    driver->phase = phase_sema_k;
    driver->sema = semantics_init(driver->parser->items, driver->source_lines, driver->lexer->source, compile_options);
    if (!sema_check(driver->sema)) {
        sema_error_flush(sema_get_diagnostics(driver->sema), driver->source_lines);
        abort_compilation(driver);
    }
    
    driver->phase = phase_codegen_k;
    JBuffer* compiled_file = cctx_get_output(sema_get_cctx(driver->sema));   

    emit_file(compile_options, compiled_file);
    
    driver_free(driver);
}
