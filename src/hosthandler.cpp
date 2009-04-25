/*
 * cclive Copyright (C) 2009 Toni Gundogdu. This file is part of cclive.
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

#include "config.h"

#include <string>

#include "except.h"
#include "video.h"
#include "singleton.h"
#include "cmdline.h"
#include "opts.h"
#include "hosthandler.h"

#ifdef WITH_PERL // -----------------------------
#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>

EXTERN_C void xs_init(pTHX);
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

static std::string
extractTitle(const std::string &html)
{
    SV *sv_html = perl_get_sv("html", TRUE);
    Options opts = optsmgr.getOptions();

    if (opts.title_cclass_given) {
        SV     *sv_cclass = perl_get_sv("cclass", TRUE);
        sv_setpv(sv_cclass, opts.title_cclass_arg);
    }
    sv_setpv(sv_html, html.c_str());
    perl_eval_pv(script, TRUE);

    return SvPV(perl_get_sv("title", FALSE), PL_na);
}

static std::string
getPageTitle(const std::string &html)
{
    perl = perl_alloc();
    perl_construct(perl);

    const char   *args[] = {"", "-e", "0"};
    perl_parse(perl, xs_init, 3, const_cast<char**>(args), 0);
    perl_run(perl);

    std::string title = extractTitle(html);

    perl_destruct(perl);
    perl_free(perl);

    return (title);
}
#endif // WITH_PERL -----------------------------

HostHandler::HostHandler()
    : pageContent(""), props(VideoProperties())
{
}

HostHandler::~HostHandler()
{
}

void
HostHandler::parsePage(const std::string& pageContent) {
    this->pageContent = pageContent;

    // call overloaded functions
    parseId         ();
    parseLink       ();

#ifdef WITH_PERL
    if (optsmgr.getOptions().title_given)
        props.setTitle(getPageTitle(pageContent));
#endif

    this->pageContent.clear();
}

const VideoProperties&
HostHandler::getVideoProperties() const {
    return props;
}

HostHandler::
    ParseException::ParseException(const std::string& error)
        : RuntimeException(error)
{
}
