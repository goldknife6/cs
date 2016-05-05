#ifndef CS_HASHYAB_H
#define CS_HASHYAB_H
#include "csglobal.h"
typedef struct table *csH_table;

typedef unsigned long(*csH_tabhashfp)(void *key);
typedef csG_bool(*csH_tabequalfp)(void *key1,void *key2);
typedef void(*csH_tabfreefp)(void *key,void *value);
typedef void (*csH_tabshowfp)(void *key, void *value);

csH_table csH_tabempty(csH_tabhashfp hfp,csH_tabequalfp efp,csH_tabfreefp ffp);
void csH_tabinsert(csH_table tab, void *key, void *value);
void *csH_tablook(csH_table tab, void *key);
void csH_tabfree(csH_table tab);
void csH_tabdump(csH_table tab, csH_tabshowfp fp);
#endif/*!CS_HASHYAB_H*/