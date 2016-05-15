#ifndef CS_MEMORY_H
#define CS_MEMORY_H
#include "csglobal.h"
#include "csobject.h"
#include "csopcodes.h"



typedef struct csM_regin {
	size_t size;
	csO_object *obj;
} csM_regin;

typedef struct csM_proc {
	size_t size;
	csO_code *code;
} *csM_proc;

typedef struct csM_procregin {
	size_t size;
	csM_proc *code;
} csM_procregin;


extern csM_regin csM_static_regin;
extern csM_regin csM_const_regin;
extern csM_procregin csM_proc_regin;
extern void csM_load_bytecode(FILE *in);
#endif/*!CS_MEMORY_H*/