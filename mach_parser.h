#include "mach_arch.h"
#include "mach_type.h"

static void mach_parser(FILE * fp, base * bs, int bit, int end)
{
	fbyte tok = 0;
	
	advance(&tok, 4, fp);
	
	bpair_parser(bs->arch, tok, mach_arches, sizeof mach_arches / sizeof(bpr), "cpu type");
	
	fbyte cls = (tok >> 24);
	
	if (cls == 0x00)	
	{
		strcpy(bs->class, "32 bit");
	}
	else if (cls == 0x1)
	{
		strcpy(bs->class, "64 bit");
	}
	else if (cls == 0x2)
	{
		strcpy(bs->class, "LP 32 bit");
	}
	else
	{
		sprintf(bs->class, "unknown (%x)", cls);
	}
	
	if (end == 0)
	{
		strcpy(bs->endian, "little endian");
	}
	else if (end == 1)
	{
		strcpy(bs->endian, "big endian");
	}
	else
	{
		strcpy(bs->endian, "unknown endian");
	}
	
	advance(&tok, 4, fp);
	advance(&tok, 4, fp);
	
	spair_parser(bs->type, tok, mach_types, sizeof mach_types / sizeof(spr), "type");
}
