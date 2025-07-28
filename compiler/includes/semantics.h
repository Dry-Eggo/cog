#pragma once
#include <compiler.h>
#include <compiler_opt.h>
#include <c_backend.h>

typedef struct Semantics Semantics;

Semantics* semantics_init(CJVec* items, JVec* source_lines, const char* source, CompileOptions* opts);
CJVec* sema_get_diagnostics(Semantics* sema);
void sema_free(Semantics* sema);
bool sema_check(Semantics*);
JBuffer* sema_get_tmp(Semantics* sema);
CContext* sema_get_cctx(Semantics* sema);
