#include "csglobal.h"
#include <stdarg.h>
#include <stdlib.h>

FILE *ins;		//input stream
FILE *outs;		//output stream
FILE *errors;	//error stream
FILE *debugs;	//debug stream
csG_string filename;

void testhash();
void testlex();
void testsym();
int main(int argc, char *argv[])
{
	outs = stdout;
	ins = stdout;
	errors = stdout;
	debugs = stdout;

	if (argc != 2) emitError("usage: %s <filename>",argv[0]);
	filename = argv[1];
	ins= fopen(filename,"r");
	if (ins == NULL) emitError("File %s not found",filename);
	setvbuf(outs, NULL, _IONBF, 0);
	setvbuf(errors, NULL, _IONBF, 0);
	setvbuf(debugs, NULL, _IONBF, 0);

	//testhash();
	//testlex();
	//testsym();
	return 0;
}

void emitError(char *message,...)
{
	va_list ap;
	VERIFY(message);
	if (filename) fprintf(stderr,"%s:",filename);
	va_start(ap,message);
	vfprintf(stderr, message, ap);
	va_end(ap);
	fprintf(stderr,"\n");
	exit(1);
}
