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

#ifdef OSNAME
# undef OSNAME /* Perl sets its own */
#endif

#include <EXTERN.h>
#include <perl.h>
#include <curl/curl.h>

#include "cclive.h"

EXTERN_C void xs_init (pTHX);
static PerlInterpreter *perl;

static char *
extract_title (char *html) {
    const char eval[] =
        "binmode (STDOUT, ':utf8');"
        "use URI::Escape qw(uri_unescape);"
        "use HTML::TokeParser;"
        "$html = uri_unescape($html);"
        "$p = HTML::TokeParser->new(\\$html);"
        "$p->get_tag('title');"
        "$title = $p->get_trimmed_text;"
        /* remove title crap */
        "$title =~ s/(youtube|video|liveleak.com|sevenload|dailymotion)//gi;"
        /* apply character-class */
        "$re = $cclass || qr|\\w|;"
        "$title = join('',$title=~/$re/g);";

    char *escaped = curl_easy_escape(cc.curl, html, 0);
    STRLEN n_a;
    char *str;

    asprintf(&str, "$html = '%s';", escaped);
    perl_eval_pv(str,TRUE);
    curl_free(escaped);
    FREE(str);

    if (cc.gi.title_cclass_given) {
        asprintf(&str, "$cclass = '%s';", cc.gi.title_cclass_arg);
        perl_eval_pv(str,TRUE);
        FREE(str);
    }

    perl_eval_pv(eval,TRUE);
    str = SvPV(perl_get_sv("title",FALSE), n_a);
    return(strdup(str));
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
