/*
 * Copyright (C) 2009 Toni Gundogdu.
 *
 * This file is part of cclive.
 * 
 * cclive is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * cclive is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// NOTE: In a parallel universe, we are using a persistent interpreter.

#include "config.h"

#include <string>

#include "except.h"
#include "opts.h"
#include "video.h"
#include "pl.h"

PerlMgr::PerlMgr()
    : pi(0)
{
}

// Keeps -Weffc++ happy.
PerlMgr::PerlMgr(const PerlMgr& o)
    : pi(o.pi)
{
}

// Ditto.
PerlMgr&
PerlMgr::operator=(const PerlMgr& o) {
    return *this;
}

PerlMgr::~PerlMgr() {
    _cleanup();
}

EXTERN_C void xs_init(pTHX);

void
PerlMgr::_init() {
    pi  = perl_alloc();
    perl_construct(pi);

    const char *args[] = {"", "-e", "0"};
    int rc = perl_parse(pi, xs_init, 3, const_cast<char**>(args), 0);

    if (rc)
        throw RuntimeException(CCLIVE_SYSTEM, "perl_parse: " + rc);

    if ((rc = perl_run(pi) != 0))
        throw RuntimeException(CCLIVE_SYSTEM, "perl_run: " + rc);
}

void
PerlMgr::_cleanup() {
    if (pi != 0) {
        PL_perl_destruct_level = 1;

        perl_destruct(pi);
        perl_free(pi);

        pi = 0;
    }
}

static const char parsetitle_pl[] =
"binmode(STDOUT, \":utf8\");"

"use Encode qw(from_to decode_utf8 FB_CROAK);"
"use HTML::TokeParser;"

"from_to($html, $1, \"utf8\")"
"    if ($html =~ /charset=(.*?)\"/);"

"$html = Encode::decode_utf8($html, Encode::FB_CROAK);"

"$parser = HTML::TokeParser->new(\\$html);"
"$parser->get_tag('title');"
"$title = $parser->get_trimmed_text;"
;

void
PerlMgr::parseTitle(const std::string& html,
                    VideoProperties& props)
{
    _init();

    SV *sv_html = perl_get_sv("html", TRUE);
    sv_setpv(sv_html, html.c_str());

    perl_eval_pv(parsetitle_pl, TRUE);

    if (SvTRUE(ERRSV))
        throw RuntimeException(CCLIVE_SYSTEM, "perl_eval_pv: failed");

    SvREFCNT_dec(sv_html);

    props.setTitle(SvPV(perl_get_sv("title", FALSE), PL_na));
}

// Presumes $title still exists from parsetitle_pl.
static const char filtertitle_pl[] =
"$title =~ s/(youtube|liveleak.com|sevenload|dailymotion)//gi;"
"$title =~ s/(on vimeo|.golem.de|clipfish|funny hub)//gi;"
"$title =~ s/(videos|video)//gi;"
"$title =~ s/^[-\\s]+//;"
"$title =~ s/\\s+$//;"

"if (!$no_cclass) {"
"    $re = $cclass || qr|\\w|;"
"    $title = join('', $title =~ /$re/g);"
"}"
;

void
PerlMgr::filterTitle(VideoProperties& props) {

    Options opts = optsmgr.getOptions();

    SV *sv_title = perl_get_sv("title", TRUE);
    sv_setpv(sv_title, props.getTitle().c_str());

    if (opts.cclass_given) {
        SV *sv_cclass = perl_get_sv("cclass", TRUE);
        sv_setpv(sv_cclass, opts.cclass_arg);
    }

    if (opts.no_cclass_given) {
        SV *sv_nocclass = perl_get_sv("no_cclass", TRUE);
        sv_setiv(sv_nocclass, 1);
    }

    perl_eval_pv(filtertitle_pl, TRUE);

    if (SvTRUE(ERRSV))
        throw RuntimeException(CCLIVE_SYSTEM, "perl_eval_pv: failed");

    props.setTitle(SvPV(get_sv("title", FALSE), PL_na));

    _cleanup();
}
