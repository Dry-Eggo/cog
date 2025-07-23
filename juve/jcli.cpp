#include <juve_utils.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct jcli_args {
    juve_map_t* options;
    juve_vec_t* positionals;
    juve_vec_t* flags;
    const char* program_name;
};

char* shift(int *i, char** v) { return v[(*i)++]; }

jcli_args_t* jcli_new_a(juve_arena_t* arena) {
    if (!arena) return nullptr;
    jcli_args_t* args = (jcli_args_t*)jarena_alloc(arena, sizeof(jcli_args_t));
    args->options = jmap_new();
    args->positionals = jvec_new();
    args->flags = jvec_new();
    return args;
}

void jcli_parse(int c, char** v, jcli_args_t* cli, juve_arena_t* arena) {
    if (!cli) return;
    int i = 0;
    cli->program_name = shift(&i, v);
    
    while (i < c) {
	char* arg = shift(&i, v);
	if (strlen(arg) >= 2 && (arg[0] == '-' && arg[1] != '-')) {
	    arg++;
	    if (strlen(arg) != 1) {
		// TODO: collect each char of the arg and set them
		fprintf(stderr, "juve cli: expected singular char name: '%s'\n", arg);
		abort();
	    }

	    if (i < c) {
		char* value = v[i];
		if (value[0] != '-') {
		    jmap_put(cli->options, arg, (void*)jarena_strdup(arena, value));
		    shift(&i, v);
		    continue;
		}
	    }	    
	    jvec_push(cli->flags, (void*)jarena_strdup(arena, arg));
	} else if (strlen(arg) >= 2 && (arg[0] == '-' && arg[1] == '-')) {
	    arg += 2;
	    char* long_name = arg;
	    if (i < c) {
		char* value = v[i];
		if (value[0] != '-') {
		    jmap_put(cli->options, long_name, (void*)jarena_strdup(arena, value));
		    shift(&i, v);
		    continue;
		}
	    }
	    jvec_push(cli->flags, (void*)jarena_strdup(arena, long_name));
	} else if (arg[0] != '-') {
	    jvec_push(cli->positionals, (void*)jarena_strdup(arena, arg));
	}
    }
}

bool jcli_has_flag(jcli_args_t* args, const char* name) {
    if (!args || !name || !args->flags) return false;
    size_t max = jvec_len(args->flags);
    for (int i = 0; i < max; ++i) {
	char* flag = (char*)jvec_at(args->flags, i);
	if (strcmp(flag, name) == 0) {
	    return true;
	}
    }
    return false;
}

bool jcli_get_option(jcli_args_t* args, const char* name, char** out) {
    if (!args || !name || !args->options) return false;
    if (!jmap_has(args->options, name)) return false;

    *out = (char*)jmap_get(args->options, name);    
    return true;
}

bool jcli_has_flag_sl(jcli_args_t* args, const char* short_n, const char* long_n) {
    if (!jcli_has_flag(args, short_n)) {
	if (!jcli_has_flag(args, long_n)) return false;
    }
    return true;
}

bool jcli_get_option_sl(jcli_args_t* args, const char* short_n, const char* long_n, char** out) {
    if (!jcli_get_option(args, short_n, out)) {
	if (!jcli_get_option(args, long_n, out)) return false;
    }
    return true;    
}
