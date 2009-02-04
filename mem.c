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
#include <memory.h>
#include <assert.h>

#include "cclive.h"

static void *
_realloc (void *p, const size_t size) {
    if (p) return realloc(p,size);
    return malloc(size);
}

size_t /* write to memory handler for fetching video page data */
writemem_cb (void *ptr, size_t size, size_t nmemb, void *data) {
    size_t rsize;
    void *tmp;
    mem_t m;

    assert(ptr  != 0);
    assert(data != 0);

    m = (mem_t)data;
    rsize = size*nmemb;

    tmp = _realloc(m->p, m->size+rsize+1);
    if (tmp) {
        m->p = tmp;
        memcpy(&(m->p[m->size]), ptr, rsize);
        m->size += rsize;
        m->p[m->size] = '\0';
    }
    return(rsize);
}
