#ifndef CS_TEMP_H
#define CS_TEMP_H
#include "csglobal.h"
#include "cssymbol.h"

typedef struct t_temp_ *csT_temp;
extern csT_temp csT_newtemp(void);

typedef struct t_templist_ *csT_templist;
extern csT_templist csT_mktemplist(csT_temp h, csT_templist t);

typedef csS_symbol csT_label;


extern csT_label csT_newlabel(void);
extern csT_label csT_namedlabel(csG_string name);

extern void printTemp(csT_temp h);
/*
char* CStmpLabelstring(csT_label s);
void printLable(CSlabel h);
*/
#endif/*CS_TEMP_H*/