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

csO_object csS_string_empty(int size)
{
	VERIFY(size >= 0);
	csO_object foo = csO_empty_object();
	foo->kind = csO_string_;
	csO_string *s = &foo->u.sval;
	s->s = malloc(size + 1);
	s->s[size] = 0;
	INIT_HLIST_NODE(&s->hash);
	s->size = size;
	s->write = TRUE;
	s->hashval = 0;
	return foo;
}

csO_object csS_string_cpy(csO_object string)
{
	VERIFY(string);
	size_t size = string->u.sval.size;
	csO_object foo = csO_empty_object();
	foo->kind = csO_string_;
	csO_string *s = &foo->u.sval;
	s->s = malloc(size + 1);
	s->s[size] = 0;
	INIT_HLIST_NODE(&s->hash);
	strncpy(s->s,string->u.sval.s,size);
	s->size = size;
	s->write = TRUE;
	s->hashval = s_strhash_(s->s,size);
	return foo;
}

char* csS_string_extend(csO_object string,size_t size)
{
	VERIFY(string);
	VERIFY(string->kind == csO_string_);
	if (!string->u.sval.write)
		VERIFY(0);
	csO_string *s = &string->u.sval;
	if (s)
		free(s->s);
	s->s = malloc(size + 1);
	s->s[size] = 0;
	INIT_HLIST_NODE(&s->hash);
	s->size = size;
	s->write = TRUE;
	s->hashval = 0;
	return s->s;
}

csO_object csO_string_object_add(csO_object s1,csO_object s2)
{
	static char buffer[10240];
	VERIFY(s1);
	VERIFY(s2);
	VERIFY(s1->kind == csO_string_);
	VERIFY(s2->kind == csO_string_);
	size_t size = s1->u.sval.size + s2->u.sval.size;
	VERIFY(size < 10240);
	sprintf(buffer, "%s%s",s1->u.sval.s,s2->u.sval.s);
	return csS_string(buffer,size);
}