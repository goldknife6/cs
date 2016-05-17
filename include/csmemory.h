#ifndef CS_MEMORY_H
#define CS_MEMORY_H
#include "csglobal.h"
#include "csobject.h"
#include "csopcodes.h"



#define CONST_REGIN_MAX 	1000
#define STATIC_REGIN_MAX 	1000
#define STACK_REGIN_MAX 	1000
#define RECORD_REGIN_MAX 	1000
#define PROC_REGIN_MAX		1000

typedef struct m_record_ {
	int m_fp_;
	int m_ip_;
	int m_funcnum_;
} csM_record;

typedef struct m_proc_ {
	size_t size;
	csO_code *ins;
} csM_proc;



extern csO_object csM_const_regin[];
extern csO_object csM_static_regin[];
extern csO_object csM_stack_regin[];
extern csM_record csM_record_regin[];

extern void csM_load_bytecode(FILE *in);
#endif/*!CS_MEMORY_H*/