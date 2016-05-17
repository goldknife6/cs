#ifndef CS_TEMP_H
#define CS_TEMP_H
#include "csglobal.h"
#include "cssymbol.h"

typedef struct t_lable_ *csT_label;



extern csT_label csT_newlabel(void);
extern void csT_plabel(csT_label lab);
#endif/*CS_TEMP_H*/