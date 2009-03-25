/*
 * cclive Copyright (C) 2009 Toni Gundogdu.
 * This file is part of cclive.
 *
 * cclive is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cclive is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <EXTERN.h>
#include <perl.h>
/* added manually: caused compilation errors on debian systems otherwise */
#include <XSUB.h>

#include "cclive.h"

EXTERN_C void xs_init (pTHX);
static PerlInterpreter *perl;

static const char script[] =
"use HTML::TokeParser;"
"$parser = HTML::TokeParser->new(\\$html);"
"$parser->get_tag('title');"
"$title = $parser->get_trimmed_text;"
"$title =~ s/(youtube|video|liveleak.com|sevenload|dailymotion)//gi;"
"$title =~ s/^\\s+//;"
"$title =~ s/\\s+$//;"
"$re = $cclass || qr|\\w|;"
"$title = join('', $title =~ /$re/g);";

static char *
extract_title (char *html) {
    SV *sv_html = perl_get_sv("html", TRUE);
    char *title = 0;

    if (cc.gi.title_cclass_given) {
        SV *sv_cclass = perl_get_sv("cclass", TRUE);
        sv_setpv(sv_cclass, cc.gi.title_cclass_arg);
    }

    sv_setpv(sv_html, html);
    perl_eval_pv(script, TRUE);

    title = SvPV(perl_get_sv("title", FALSE), PL_na);

    return (title
        ? strdup(title)
        : NULL);
}

char *
page_title (char *html) {
    char *args[] = {"", "-e", "0"};
    char *title = 0;

    if (!cc.gi.title_given)
        return(title);

    perl = perl_alloc();
    perl_construct(perl);

    perl_parse(perl, xs_init, 3, args, 0);
    perl_run(perl);

    title = extract_title(html);

    perl_destruct(perl);
    perl_free(perl);

    return(title);
}
