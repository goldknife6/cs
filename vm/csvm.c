#include "csvm.h"
#include "csmemory.h"

static void v_exe_(csM_regin *cons,csM_regin *stat,csM_procregin *proc);


void main(int argc, char *argv[])
{
	csG_string filename;
	if (argc != 2) 
		fprintf(stderr,"usage: %s <filename>",argv[0]);
	filename = argv[1];
	FILE *ins = fopen(filename,"r");
	if (ins == NULL) 
		fprintf(stderr,"File %s not found",filename);


	csM_load_bytecode(ins);
	v_exe_(&csM_const_regin,&csM_static_regin,&csM_proc_regin);
}

	
static void v_exe_(csM_regin *cons,csM_regin *stat,csM_procregin *proc)
{
	
}