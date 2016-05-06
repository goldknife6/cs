#ifndef CS_LEX_H
#define CS_LEX_H
#include "csglobal.h"

typedef enum {
		csL_ID = 1,csL_STRING,csL_NUM,csL_CHAR,csL_APOS,/*'*/
		/* special symbols */
		csL_COMMA,csL_COLON,csL_SEMICOLON,csL_LPAREN,csL_RPAREN,csL_LBRACK,csL_RBRACK,csL_LBRACE,csL_RBRACE,csL_DOT,csL_DOLLAR,csL_NOT,
		/* ,		:			;			(			)			[			]		{				}	 .	  		$		! */
		csL_PLUS,csL_MINUS,csL_TIMES,csL_DIVIDE,csL_EQ,csL_NEQ,csL_LT,csL_LE,csL_GT,csL_GE,csL_AND,csL_OR,csL_ASSIGN,
		/*+		-	*		/	==  !=  < <=  > >=  &   |    =		*/
		csL_IF,csL_ELSE,csL_WHILE,csL_FOR,csL_BREAK,csL_DEF,csL_TRUE/*true*/,csL_FALSE/*false*/,csL_RETURN,csL_VAR,
		/* reserved words */
		csL_ENDFILE,csL_ERROR
} csL_tokkind;

typedef struct {
	csL_tokkind kind;
	csG_pos pos;
	union {
		int ival;
		csG_bool bval;
		csG_string sval;
		int cval;
	} u;
} csL_token;

extern csL_token csL_gettoken();
extern void csL_prttoken(csL_token token);
extern csG_string csL_tokenstr(csL_token token);
extern int csL_tokenchar(csL_token token);
extern int csL_tokennum(csL_token token);
extern csG_bool csL_tokenbool(csL_token token);
#endif/*!CS_LEX_H*/