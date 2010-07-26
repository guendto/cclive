/*
 * Copyright (C) 2009,2010 Toni Gundogdu.
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

#ifndef opts_h
#define opts_h

#include "singleton.h"
#include "cmdline.h"

typedef gengetopt_args_info Options;

class OptionsMgr : public Singleton<OptionsMgr> {
public:
    OptionsMgr();
    virtual ~OptionsMgr();
public:
    void                init        (int,char **);
    const Options&      getOptions  () const;
    const std::string&  getPath     () const;
    const std::string&  getLocale   () const;
    const bool&         getToUnicodeFlag() const;
private:
    Options opts;
    std::string path;
    std::string locale;
};

#define optsmgr OptionsMgr::getInstance()

#endif


