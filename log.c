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
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#include "cclive.h"

int /* general purpose log function; notice stderr */
cc_log (const char *fmt, ...) {
    va_list args;
    char *p = 0;

    assert(fmt != 0);

    if (cc.gi.quiet_given)
        return(0);
    
    va_start(args,fmt);
    vasprintf(&p,fmt,args);
    va_end(args);

    if (p)
        fprintf(stderr,"%s",p);
    free(p);

    return(0);
}
