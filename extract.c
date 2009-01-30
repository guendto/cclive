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
#include <assert.h>

#include "cclive.h"

int /* prepare video file for extraction */
cc_extract_video (char *xurl, char *id, char *host) {
    double len,initial;
    char *fn,*ct;
    int rc;

    assert(xurl != 0);
    assert(id   != 0);
    assert(host != 0);

    initial = 0;

    rc = cc_getlen(xurl, &len, &ct);
    if (!rc) {
        fn = cc_getfn(&initial, len, id, cc.gi.download_arg, host);
        if (fn) {
            rc = 0;
            if (cc.gi.no_extract_given)
                cc_log("%s  %.2fMB  [%s]\n",fn,ToMB(len),ct);
            else if (cc.gi.emit_csv_given)
                fprintf(stdout,"csv:\"%s\",\"%.0f\",\"%.0f\",\"%s\"\n",
                    fn,len,initial,xurl);
            else
                rc = cc_get(xurl, fn, initial, len);
            free(fn);
        }
    }
    return(rc);
}
