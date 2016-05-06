#ifndef CS_GLOBAL_H
#define CS_GLOBAL_H
#include <assert.h>
#include <stdio.h>

#ifdef CS_DEBUG_PTR
# define VERIFY(x) assert(x)
#else
# define VERIFY(x) 
#endif/*!CS_DEBUG_PTR*/

#ifndef FALSE
# define FALSE 0
#endif

#ifndef TRUE
# define TRUE 1
#endif

typedef char *csG_string;
typedef unsigned int csG_bool;

extern FILE *ins;		//input stream
extern FILE *outs;		//output stream
extern FILE *errors;	//error stream
extern FILE *debugs;	//debug stream
extern csG_string filename;

typedef struct {
	int row;
	int col;
} csG_pos;

extern csG_bool csG_error;

extern void emitError(char *message,...);
#endif/*!CS_GLOBAL_H*/