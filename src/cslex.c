#include "cslex.h"
#include "cshash.h"
#include "csutil.h"
#include <ctype.h>
#include <stdlib.h>

/* MAXTOKENLEN is the maximum size of a token */
#define MAXTOKENLEN 128

typedef enum {
	START,DONE,
	INID,INNUM,INSTRING,INCOMMENT1,INCOMMENT2,INCOMMENT3,INCHAR
} state;

static int row = 1,col = 1;//line number and column number

static csH_table keytab;
static void keyinit();
static csL_tokkind keylookup(char *s);
static int get();
static void unget(int c);

csL_token csL_gettoken()
{
	int c,index = 0;
	static csG_bool flag = FALSE;
	csL_tokkind kind;
	state currstate = START;
	char strbuffer[MAXTOKENLEN+1];
	if (!flag) {
		keyinit();
		flag = TRUE;
	}
	while (currstate != DONE) {
		int save = TRUE;
		c = get();
		switch (currstate) {
		case START:
			if (isdigit(c))
           		currstate = INNUM;
         	else if (isalpha(c))
           		currstate = INID;
         	else if ((c == ' ') || (c == '\t') || (c == '\n'))
           		save = FALSE;
         	else if (c == '/') { 
         		save = FALSE;
				currstate = INCOMMENT1;
			} else if (c == '"') { 
         		save = FALSE;
				currstate = INSTRING;
			} else if (c == '\'') { 
         		save = FALSE;
				currstate = INCHAR;
			} else {
				currstate = DONE;
				switch (c) {
				case EOF:
					kind = csL_ENDFILE;break;
				case '+':
					kind = csL_PLUS;break;
				case '-':
					kind = csL_MINUS;break;
				case '*':
					kind = csL_TIMES;break;
				case '!':
				{
					kind = csL_NOT;
					char ch = get();
					if (ch == '=') kind = csL_NEQ;
					else unget(ch);
					break;
				}
				case '$':
					kind = csL_DOLLAR;break;		
				case '(':
					kind = csL_LPAREN;break;
				case ')':
					kind = csL_RPAREN;break;
				case '{':
					kind = csL_LBRACE;break;
				case '}':
					kind = csL_RBRACE;break;
				case '[':
					kind = csL_LBRACK;break;
				case ']':
					kind = csL_RBRACK;break;
				case ';':
					kind = csL_SEMICOLON;break;
				case '<':
				{
					kind = csL_LT;
					char ch = get();
					if (ch == '=') kind = csL_LE;
					else unget(ch);
					break;
				}
				case '>':
				{
					kind = csL_GT;
					char ch = get();
					if (ch == '=') kind = csL_GE;
					else unget(ch);
					break;
				}
				case ',':
					kind = csL_COMMA;break;
				case '&':
					kind = csL_AND;break;
				case '|':
					kind = csL_OR;break;
				case '=':
				{
					kind = csL_ASSIGN;
					char ch = get();
					if (ch == '=') kind = csL_EQ;
					else unget(ch);
					break;
				}
				case ':':
					kind = csL_COLON;break;
				default:
					kind = csL_ERROR;
               		break;
				}
			}
			break;
		case INNUM:
			if (!isdigit(c)) {
				currstate = DONE;
				kind = csL_NUM;
				unget(c);
				save = FALSE;
			}
			break;
		case INID:
			if (!isalpha(c) && !isdigit(c)) {
				currstate = DONE;
				kind = csL_ID;
				save = FALSE;
				unget(c);
			}
			break;
		case INCOMMENT1:
			save = FALSE;
			if(c == '*') {
				currstate = INCOMMENT2;
			} else if (c == '/') { /* // comments*/
				char ch = get();
				while(ch != '\n' && ch != EOF)
					ch = get();
				unget(ch);
				currstate = START;
			} else {
				unget(c);
				currstate = DONE;
				kind = csL_DIVIDE;
			}
			break;
		case INCOMMENT2: /* comments */
			save = FALSE;
			if(c == '*') {
				currstate = INCOMMENT3;
			} else if (c == EOF) {
				currstate = DONE;
				kind = csL_ENDFILE;
			}
			break;
		case INCOMMENT3:
			save = FALSE;
			if(c == '*') {
				currstate = INCOMMENT3;
			} else if (c == '/') {
				currstate = START;		
			} else {
				currstate = INCOMMENT2;			
			}	
			break;
		case INSTRING:
			if (c == '"') {
				save = FALSE;
				currstate = DONE;
				kind = csL_STRING;
			}
			break;
		case INCHAR:{
			int ch = get();
			if (ch != '\'') {
				index = 0;
				save = FALSE;
				currstate = START;
				while (ch != '\'' && ch != EOF)
					ch = get();
			} else {
				currstate = DONE;
				kind = csL_CHAR;
			}
			break;
		}
		default:
			emitError("Scanner Bug: state= %d\n",currstate);
		}/*!switch (currState)*/
		if (save && index <= MAXTOKENLEN)
			strbuffer[index++] = (char) c;
		if (index > MAXTOKENLEN)
			VERIFY(0);
		if (currstate == DONE) { 
			strbuffer[index] = '\0';
			if (kind == csL_ID)
       			kind = keylookup(strbuffer);
     	}
   	}/*!while*/

    csL_token token;
    token.kind = kind;
    token.pos.row = row;
	token.pos.col = col;
	if (kind == csL_NUM)
		token.u.ival = atoi(strbuffer);
	else if (kind == csL_ID || kind == csL_STRING)
		token.u.sval = csU_strdup(strbuffer);
	else if (kind == csL_CHAR)
		token.u.cval = strbuffer[0];
	return token;
}

static int lastcol;
static int get() 
{
	int c = fgetc(ins);
	if (c == '\n') {
		lastcol = col;
		row++,col = 1;
	} else if (c == '\t') {
		col += 2;
	} else {
		col++;
	}	
	return c;
}

static void unget(int c) 
{ 
	int res = ungetc(c, ins);
	if (res != c)
		emitError("unget failed:%c",c);
	if (c == '\n') {
		row--;
		col = lastcol;
	} else if (c == '\t') {
		col -= 2;
	} else {
		col--;
	}
}

static void keyinit() 
{
	keytab = csH_tabempty(csU_strhash,csU_strequal,NULL);
	csH_tabinsert(keytab, "if", (void*)csL_IF);
	csH_tabinsert(keytab, "else", (void*)csL_ELSE);
	csH_tabinsert(keytab, "while", (void*)csL_WHILE);
	csH_tabinsert(keytab, "for", (void*)csL_FOR);
	csH_tabinsert(keytab, "break", (void*)csL_BREAK);
	csH_tabinsert(keytab, "def", (void*)csL_DEF);
	csH_tabinsert(keytab, "true", (void*)csL_TRUE);
	csH_tabinsert(keytab, "false", (void*)csL_FALSE);
	csH_tabinsert(keytab, "return", (void*)csL_RETURN);
	csH_tabinsert(keytab, "var", (void*)csL_VAR);
}

static csL_tokkind keylookup(char *s) 
{
	VERIFY(s);VERIFY(keytab);
	void *k =  csH_tablook(keytab, s);
	if (!k) 
		return csL_ID;
	return (csL_tokkind)k;
}


void csL_prttoken(csL_token token) 
{
	csL_tokkind kind = token.kind;
	switch(kind) {
	case csL_ID: case csL_STRING:	
		fprintf(debugs,"%s ",token.u.sval);
		break;
	case csL_NUM:
		fprintf(debugs,"%d ",token.u.ival);
		break;
	case csL_CHAR:
		fprintf(debugs,"%c ",token.u.cval);
		break;
	default:
		fprintf(debugs,"%d ",kind);
	}
}