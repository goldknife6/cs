#include "csobject.h"
#include "cslist.h"
#include <string.h>
#include <stdlib.h>

#define HASHSIZE 1024

static csL_hlisth s_head_[HASHSIZE];

static unsigned long s_strhash_(char *s,size_t size);

static csO_object s_find_(char *s,size_t size)
{
	VERIFY(s);VERIFY(size >= 0);
	csL_hlisth *head = &s_head_[s_strhash_(s,size) % HASHSIZE];
	csO_object pos;
	hlist_for_each_entry(pos, head, u.sval.hash) {
		if (pos->u.sval.size != size)
			continue;
		if (!strncmp(pos->u.sval.s,s,size))
			return pos;
	}
	return NULL;
}

static unsigned long s_strhash_(char *s,size_t size)
{
	unsigned long h = 0;
	for(int i = 0; i < size; i++)  
       h = h*65599 + s[i];
	return h;
}

csO_object csS_string(char *src,int size)
{
	VERIFY(src);
	VERIFY(size >= 0);
	csO_object foo = s_find_(src,size);
	if (foo) {
		foo->u.sval.write = FALSE;
		return foo;
	}
	foo = csO_empty_object();
	foo->kind = csO_string_;
	csO_string *s = &foo->u.sval;
	s->s = malloc(size + 1);
	s->s[size] = 0;
	INIT_HLIST_NODE(&s->hash);
	strncpy(s->s,src,size);
	s->size = size;
	s->write = TRUE;
	s->hashval = s_strhash_(src,size);
	csL_hlisth *head = &s_head_[s->hashval % HASHSIZE];
	hlist_add_head(&s->hash, head);
	return foo;
}


