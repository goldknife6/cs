#include "csparser.h"
#include "csutil.h"

static void p_declist(csA_declist dec);
static void p_dec(csA_dec dec);

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
	if (!dec) return;
	csG_pos p = csA_decpos(dec);
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
		VERIFY(0);
		break;
	default:
		VERIFY(0);
	}	
}