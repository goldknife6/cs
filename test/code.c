#include "cscode.h"
#include "csparser.h"
#include "csenv.h"

static void c_printaddr(csC_address addr);
static void c_printquad(csC_quad quad);
static void c_printfrag(csC_frag frag);
void csC_printcode(csC_fraglist fraglist);
void testcode(void)
{
	csA_declist list = parser();
	csS_table val = csE_baseval();
	csS_table type = csE_basetype();
	csC_fraglist fraglist = c_declist_(val,type,list);
	if (!csG_error && fraglist)
		csC_printcode(fraglist);
}
void csC_printcode(csC_fraglist fraglist)
{
	csC_frag pos = NULL;
	list_for_each_entry(pos, fraglist, next) {
		c_printfrag(pos);
	}
}
static void c_printaddr(csC_address addr) 
{
	switch (addr.kind) {
	case csC_empty:	
		break;
	case csC_intconst:
		fprintf(debugs, "%d",addr.u.ival);
		break;
	case csC_strconst:	
		fprintf(debugs, "%s",addr.u.str);
		break;
	case csC_boolconst:	
		fprintf(debugs, "%s",addr.u.bval ? "TRUE" : "FALSE");
		break;
	case csC_env:
		VERIFY(addr.u.eval);
		fprintf(debugs, "%s",csS_name(addr.u.eval->name));	
		break;
	case caC_lable:	
		csT_plabel(addr.u.lab);
		break;
	default:
		VERIFY(0);
	}
}

static void c_printquad(csC_quad quad)
{
	VERIFY(quad);

	switch (quad->kind) {
	case csC_entry:
		fprintf(debugs, "entry ");
		break;
	case csC_load:
		fprintf(debugs, "load ");
		break;
	case csC_loadaddr:
		fprintf(debugs, "loadaddr ");
		break;
	case csC_stores:
		fprintf(debugs, "stores ");
		break;
	case csC_storel:
		fprintf(debugs, "storel ");
		break;
	case csC_add:
		fprintf(debugs, "add ");
		break;
	case csC_sub:
		fprintf(debugs, "sub ");
		break;
	case csC_mul:
		fprintf(debugs, "mul ");
		break;
	case csC_div:
		fprintf(debugs, "div ");
		break;
	case csC_eq:
		fprintf(debugs, "eq ");
		break;
	case csC_neq:
		fprintf(debugs, "neq ");
		break;
	case csC_lt:
		fprintf(debugs, "lt ");
		break;
	case csC_lq:
		fprintf(debugs, "lq ");
		break;
	case csC_gt:
		fprintf(debugs, "gt ");
		break;
	case csC_gq:
		fprintf(debugs, "gq ");
		break;
	case csC_fjp:
		fprintf(debugs, "fjp ");
		break;
	case csC_tjp:
		fprintf(debugs, "tjp ");
		break;
	case csC_ujp:
		fprintf(debugs, "ujp ");
		break;
	case csC_lab:
		fprintf(debugs, "lab ");
		break;
	case csC_mark:
		fprintf(debugs, "mark");
		break;
	case csC_cup:
		fprintf(debugs, "cup ");
		break;
	case csC_cbp:
		fprintf(debugs, "cbp ");
		break;
	case csC_ret:
		fprintf(debugs, "ret ");
		break;
	case csC_or:
		fprintf(debugs, "or ");
		break;
	case csC_and:
		fprintf(debugs, "and ");
		break;
	case csC_minus:
		fprintf(debugs, "minus ");
		break;
	case csC_not:
		fprintf(debugs, "not ");
		break;
	case csC_ssp:
		fprintf(debugs, "ssp ");
		break;
	case csC_prv:
		fprintf(debugs, "prv");
		break;
 	default:
		VERIFY(0);
	}
	c_printaddr(quad->arg);
	fprintf(debugs, "\n");
}

static void c_printfrag(csC_frag frag)
{
	if (!frag) return;
	int offset;
	switch (frag->kind) {
	case csC_strfrag:
		offset = frag->offset;
		fprintf(debugs, "static string:%s ", frag->u.strv);
		fprintf(debugs, "offset:%d\n",offset);
		break;
	case csC_intfrag:
		offset = frag->offset;
		fprintf(debugs, "static int:%d ", frag->u.intv);
		fprintf(debugs, "offset:%d\n",offset);
		break;
	case csC_boolfrag:
		offset = frag->offset;
		fprintf(debugs, "static bool:%d ", frag->u.boolv);
		fprintf(debugs, "offset:%d\n",offset);
		break;
	case csC_procfrag:{
		csC_quad pos;
		VERIFY(frag->u.proc.body);
		list_for_each_entry(pos, frag->u.proc.body, next) {
			c_printquad(pos);
		}
		break;
	}
	default:
		VERIFY(0);
	}
}
