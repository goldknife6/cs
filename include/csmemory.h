#ifndef CS_MEMORY_H
#define CS_MEMORY_H
#include "csglobal.h"
#include "csobject.h"



struct csM_static_regin {
	size_t size;
	csO_object *obj;
};





extern void csM_load_bytecode(FILE *in);
#endif/*!CS_MEMORY_H*/