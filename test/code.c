#include "cscode.h"
#include "csparser.h"
#include "csenv.h"

static void c_printaddr(csC_address addr);
static void c_printquad(csC_quad quad);
static void c_printfrag(csC_frag frag);
static void csC_printcode();
void testcode(void)
{
	csA_declist list = parser();
	csS_table val = csE_baseval();
	csS_table type = csE_basetype();
	c_declist_(val,type,list);

	csC_printcode();
}
static void csC_printcode()
{
	csC_frag pos = NULL;
	list_for_each_entry(pos, &fraglist, next) {
		c_printfrag(pos);
	}
}
static void c_printaddr(csC_address addr) 
{
	switch (addr.kind) {
	case csC_empty:	
		fprintf(debugs, "empty address");
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
	case csC_temp:
		VERIFY(addr.u.tmp);
		printTemp(addr.u.tmp);	
		break;
	case caC_lable:	
		VERIFY(addr.u.lab);
		fprintf(debugs, "%s",csS_name(addr.u.lab));
		break;
	default:
		VERIFY(0);
	}
}

static void c_printquad(csC_quad quad)
{
	VERIFY(quad);

	switch (quad->kind) {
 	case csC_lable:
 		fprintf(debugs, "labal: ");
 		c_printaddr(quad->res);
 		break;
 	case csC_assign:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = ");
 		c_printaddr(quad->arg1);
 		break;
 	case csC_goto:
 		fprintf(debugs, "goto ");
 		c_printaddr(quad->res);
 		break;
 	case csC_iffalse:
 		fprintf(debugs, "iffalse ");
 		c_printaddr(quad->arg1);
 		fprintf(debugs, " goto ");
 		c_printaddr(quad->res);
 		break;
 	case csC_if:
 		fprintf(debugs, "if ");
 		c_printaddr(quad->arg1);
 		fprintf(debugs, " goto ");
 		c_printaddr(quad->res);
 		break;
 	case csC_add:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = ");
 		c_printaddr(quad->arg1);
 		fprintf(debugs, " + ");
 		c_printaddr(quad->arg2);
 		break;
 	case csC_call:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = call ");
 		c_printaddr(quad->arg1);
 		fprintf(debugs, ",");
 		c_printaddr(quad->arg2);
 		break;
 	case csC_return:
 		fprintf(debugs, "return ");
 		c_printaddr(quad->res);
 		break;
 	case csC_param:
 		fprintf(debugs, "param ");
 		c_printaddr(quad->res);
 		break;
 	case csC_minus:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = -");
 		c_printaddr(quad->arg1);
 		break;
 	case csC_multiply:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = ");
 		c_printaddr(quad->arg1);
 		fprintf(debugs, " * ");
 		c_printaddr(quad->arg2);
 		break;
 	case csC_divide:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = ");
 		c_printaddr(quad->arg1);
 		fprintf(debugs, " / ");
 		c_printaddr(quad->arg2);
 		break;
 	case csC_sub:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = ");
 		c_printaddr(quad->arg1);
 		fprintf(debugs, " - ");
 		c_printaddr(quad->arg2);
 		break;
 	case csC_or:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = ");
 		c_printaddr(quad->arg1);
 		fprintf(debugs, " | ");
 		c_printaddr(quad->arg2);
 		break;
 	case csC_and:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = ");
 		c_printaddr(quad->arg1);
 		fprintf(debugs, " & ");
 		c_printaddr(quad->arg2);
 		break;
 	case csC_eq:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = ");
 		c_printaddr(quad->arg1);
 		fprintf(debugs, " == ");
 		c_printaddr(quad->arg2);
 		break;
 	case csC_neq:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = ");
 		c_printaddr(quad->arg1);
 		fprintf(debugs, " != ");
 		c_printaddr(quad->arg2);
 		break;
 	case csC_lt:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = ");
 		c_printaddr(quad->arg1);
 		fprintf(debugs, " < ");
 		c_printaddr(quad->arg2);
 		break;
 	case csC_lq:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = ");
 		c_printaddr(quad->arg1);
 		fprintf(debugs, " <= ");
 		c_printaddr(quad->arg2);
 		break;
 	case csC_gt:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = ");
 		c_printaddr(quad->arg1);
 		fprintf(debugs, " > ");
 		c_printaddr(quad->arg2);
 		break;
 	case csC_gq:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = ");
 		c_printaddr(quad->arg1);
 		fprintf(debugs, " >= ");
 		c_printaddr(quad->arg2);
 		break;
 	case csC_not:
 		c_printaddr(quad->res);
 		fprintf(debugs, " = !");
 		c_printaddr(quad->arg1);
 		break;
 	default:
		VERIFY(0);
	}
	fprintf(debugs, "\n");
}

static void c_printfrag(csC_frag frag)
{
	if (!frag) return;
	csF_access acc = NULL;
	switch (frag->kind) {
	case csC_strfrag:
		acc = frag->access;
		VERIFY(acc);
		fprintf(debugs, "static string:%s ", frag->u.strv);
		fprintf(debugs, "%d %d\n",acc->kind,acc->u.offset);
		break;
	case csC_intfrag:
		acc = frag->access;
		VERIFY(acc);
		fprintf(debugs, "static int:%d ", frag->u.intv);
		fprintf(debugs, "%d %d\n",acc->kind,acc->u.offset);
		break;
	case csC_boolfrag:
		acc = frag->access;
		VERIFY(acc);
		fprintf(debugs, "static bool:%d ", frag->u.boolv);
		fprintf(debugs, "%d %d\n",acc->kind,acc->u.offset);
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