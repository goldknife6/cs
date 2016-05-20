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


typedef struct m_proc_ {
	size_t size;
	csO_code *ins;
} csM_proc;


extern csO_value csM_const_regin[];
extern csO_value csM_static_regin[];
extern csO_value csM_stack_regin[];
extern int csM_record_regin[];

extern csO_value csO_int_value(int val);
extern csO_value csO_bool_value(csG_bool val);
extern csO_value csO_object_value(csO_object obj);
extern void csM_store_active_record_value(int offset,csO_value v);

extern csO_code csM_nextins();
extern csO_value csM_const_value(int offset);
extern csO_value csM_static_value(int offset);
extern csO_value csM_active_record_value(int offset);
extern void csM_store_static_value(int offset,csO_value v);

extern void csM_change_proc(int proc);

extern void csM_push_record(int addr);
extern int csM_pop_record();

extern void csM_push_object(csO_object obj);
extern csO_object csM_pop_object(void);

extern csO_value csM_pop_value(void);
extern void csM_push_value(csO_value v);

extern void csM_push_address(int addr);
extern int csM_pop_address(void);

extern void csM_load_bytecode(FILE *in);

extern csO_value csM_null_value();
#endif/*!CS_MEMORY_H*/