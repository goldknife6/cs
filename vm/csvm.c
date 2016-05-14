#include "csvm.h"
#include "csmemory.h"

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
}