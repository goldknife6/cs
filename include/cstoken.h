#ifndef CS_TOKENS_H
#define CS_TOKENS_H
#include "csglobal.h"


typedef enum {
	ID = 1,STRING,NUM,CHAR,
	/* special symbols */
	COMMA,COLON,SEMICOLON,LPAREN,RPAREN,LBRACK,RBRACK,LBRACE,RBRACE,DOT,DOLLAR,NOT,
	/* ,	:		;		(		)		[	]		{		}	 .	  $		! */
	PLUS,MINUS,TIMES,DIVIDE,EQ,NEQ,LT,LE,GT,GE,AND,OR,ASSIGN,
	/*+		-	*		/	==  !=  < <=  > >=  &   |    =		*/
	/* reserved words */
	IF,ELSE,WHILE,FOR,BREAK,DEF,T/*true*/,F/*false*/,RETURN,VAR,
	ENDFILE,ERROR
} csT_tokenKind;
#endif/*!CS_TOKENS_H*/