#include "csparser.h"
#include "csutil.h"

static void p_declist(csA_declist dec);
static void p_dec(csA_dec dec);
static void p_paramlist(csA_paramlist list);
static void p_param(csA_param p);

void testparser(void)
{
	csA_declist dec = parser();
	p_declist(dec);
}

static void p_declist(csA_declist dec)
{
	if (!dec) return;
	csA_dec pos = NULL;
	list_for_each_entry(pos, dec, next) {
		VERIFY(pos);
		p_dec(pos);
			
	}

}

static void p_dec(csA_dec dec)
{
	csS_symbol s = NULL;
	csA_paramlist p = NULL;
	if (!dec) return;
	csG_pos pos = csA_decpos(dec);
	switch (dec->kind) {
	case csA_vardec:
		s = csA_decvartype(dec);
		VERIFY(s);
		fprintf(debugs, "var %s ", csS_name(s));
		s = csA_decvarname(dec);
		VERIFY(s);
		fprintf(debugs, "%s ;\n", csS_name(s));
		break;
	case csA_fundec:
		s = csA_decfunname(dec);
		VERIFY(s);
		fprintf(debugs, "def %s ", csS_name(s));
		s = csA_decfunrestype(dec);
		VERIFY(s);
		fprintf(debugs, "(");
		p = csA_decfunparamlist(dec);
		p_paramlist(p);
		fprintf(debugs, ")");
		fprintf(debugs, " %s\n", csS_name(s));
		break;
	default:
		VERIFY(0);
	}	
}

static void p_paramlist(csA_paramlist list)
{
	if (!list) return;
	csA_param pos = NULL;
	char *s = "";
	list_for_each_entry(pos, list, next) {
		fprintf(debugs,"%s", s);
		p_param(pos);
		s = ",";
	}
}

static void p_param(csA_param foo)
{
	VERIFY(foo);
	csS_symbol s1 = csA_paramtype(foo);
	csS_symbol s2 = csA_paramname(foo);
	VERIFY(s1);
	VERIFY(s2);
	fprintf(debugs, "%s %s",csS_name(s1),csS_name(s2));
}