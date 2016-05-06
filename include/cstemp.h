#ifndef CS_TEMP_H
#define CS_TEMP_H
#include "csglobal.h"


typedef struct t_temp_ *csT_temp;
csT_temp csT_newtemp(void);

typedef struct t_templist_ *csT_templist;
csT_templist csT_mktemplist(csT_temp h, csT_templist t);

typedef struct t_label_ *csT_label;

/*
csT_label CStmpNewlabel(void);
csT_label CStmpNamedlabel(char* name);
char* CStmpLabelstring(csT_label s);

typedef struct cs_labelList_ *CSlabelList;
struct cs_labelList_ { csT_label head; CSlabelList tail;};




void printTemp(CStemp h);
void printLable(CSlabel h);
*/
#endif/*CS_TEMP_H*/