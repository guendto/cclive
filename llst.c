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

#ifdef HAVE_STDLIB_H
  #include <stdlib.h>
#endif
#include <stdarg.h>
#include <assert.h>

#include "cclive.h"

int
llst_append (llst_node_t *head, const char *fmt, ...) {
    va_list args;
    char *str=0;
    int rc=1;

    assert(head != 0);

    va_start(args,fmt);
    vasprintf(&str,fmt,args);
    va_end(args);

    if (str) {
#ifdef HAVE_MALLOC
        llst_node_t n=malloc(sizeof(struct _llst_node_s));
#else
    #error TODO: malloc function missing; workaround needed
#endif
        if (n) {
            llst_node_t curr = *head;
            n->str  = str;
            n->next = 0;
            if (curr == 0)
                *head = n;
            else {
                while (curr->next != 0)
                    curr = curr->next;
                curr->next = n;
            }
            rc=0;
        }
        else {
            perror("malloc");
            FREE(str);
        }
    }
    else
        perror("vasprintf");
    return(rc);
}

void
llst_dump (llst_node_t head) {
    llst_node_t curr = head;
    while (curr != 0) {
        puts(curr->str);
        curr = curr->next;
    }
}

void
llst_free (llst_node_t *head) {
    llst_node_t curr = *head;
    while (curr != 0) {
        llst_node_t next = curr->next;
        FREE(curr->str);
        FREE(curr);
        curr = next;
    }
    *head = 0;
}
