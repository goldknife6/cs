#ifndef CS_BYTEC_H
#define CS_BYTEC_H
#include "csglobal.h"
#include "cscode.h"
#include "cslist.h"
#include "csformat.h"

typedef struct csB_codelist {
	csO_code code;
	csL_list next;
} *csB_codelist;

typedef struct csB_lablelist {
	int offset;
	csL_list next;
	csT_label lab;
	csB_codelist code;
} *csB_lablelist;

typedef struct csB_regin {
	csF_fmtkind kind;
	int size;
	int offset;
	union {
		struct {
			csL_list *head;//proc code list
			csG_bool main;
		} fun;
		struct {
			csF_fmtvalkind kind;
			union {
				csG_string strv;
				int intv;
				csG_bool boolv;
			} u;
		} val;
	} u;
	csL_list next;
}*csB_regin;
#endif/*!CS_BYTEC_H*/