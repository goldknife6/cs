#include "cshash.h"
#include "cslist.h"
#include "csutil.h"
#include <string.h>

#define TABSIZE 65599

typedef struct bucket {
	void *key; 
	void *value;
	csL_hlistn next;
} *csH_bucket;

struct table {
	csL_hlisth bucket[TABSIZE];
	csH_tabhashfp hfp;
	csH_tabequalfp efp;
	csH_tabfreefp ffp;
};

static csH_bucket bucket(void *key, void *value, csH_bucket next);
static unsigned long hash(void *key,csH_tabhashfp hashfp);
static csG_bool equal(void *key1,void *key2,csH_tabequalfp fp);

csH_table csH_tabempty(csH_tabhashfp hfp,csH_tabequalfp efp,csH_tabfreefp ffp)
{ 
	csH_table foo = csU_malloc(sizeof(*foo));
	bzero(foo,sizeof(*foo));
	foo->hfp = hfp;
	foo->efp = efp;
	foo->ffp = ffp;
	return foo;
}

void csH_tabfree(csH_table tab)
{
	VERIFY(tab);
	int b = 0;
	csH_tabfreefp ffp = tab->ffp;
	for (int i = 0;i < TABSIZE; i++) {
		csH_bucket pos = NULL,last = NULL;
		hlist_for_each_entry(pos, &tab->bucket[i], next) {
			VERIFY(pos);
			if (ffp)
				ffp(pos->key,pos->value);
			if(last) {
				hlist_del(&last->next);
				csU_free(last);
			}
			last = pos;
		}
		if(last) {
			hlist_del(&last->next);
			csU_free(last);
		}
	}
	csU_free(tab);
}

void csH_tabinsert(csH_table tab, void *key, void *value)
{
 	VERIFY(tab);
 	csH_bucket foo;
 	unsigned long index = hash(key,tab->hfp) % TABSIZE;
 	foo = bucket(key, value,NULL);
 	hlist_add_head(&foo->next, &tab->bucket[index]);
}

void *csH_tablook(csH_table tab, void *key)
{
 	VERIFY(tab);
	unsigned long index = hash(key,tab->hfp) % TABSIZE;
	csL_hlisth head = tab->bucket[index];
	csH_bucket pos = NULL;
	hlist_for_each_entry(pos, &head, next) {
		VERIFY(pos);
		if (equal(pos->key,key,tab->efp)) 
			return pos->value;
	}
	return NULL;
}

void csH_tabdump(csH_table tab, csH_tabshowfp fp)
{
	VERIFY(tab);
	for (int i = 0;i < TABSIZE; i++) {
		csH_bucket pos = NULL;
		hlist_for_each_entry(pos, &tab->bucket[i], next) {
			VERIFY(pos);
			if (fp)
				fp(pos->key,pos->value);
		}
	}
}

static csH_bucket bucket(void *key, void *value, csH_bucket next)
{
	csH_bucket b = csU_malloc(sizeof(*b));
 	b->key = key; 
 	b->value=value; 
 	INIT_HLIST_NODE(&b->next);
 	if (next)
 		hlist_add_before(&b->next,&next->next);
 	return b;
}

static unsigned long hash(void *key,csH_tabhashfp fp)
{
	if (!fp) 
		return (unsigned long)key;
 	return fp(key);
}

static csG_bool equal(void *key1,void *key2,csH_tabequalfp fp)
{
	if (!fp) 
		return key1 == key2;
 	return fp(key1,key2);
}
