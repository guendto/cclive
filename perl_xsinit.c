/* perl -MExtUtils::Embed -e xsinit -- -o perl_xsinit.c */

#ifdef OSNAME
# undef OSNAME /* Perl sets its own */
#endif

#include <EXTERN.h>
#include <perl.h>

EXTERN_C void xs_init (pTHX);

EXTERN_C void boot_DynaLoader (pTHX_ CV* cv);

EXTERN_C void
xs_init(pTHX)
{
	char *file = __FILE__;
	dXSUB_SYS;

	/* DynaLoader is a special case */
	newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
}
