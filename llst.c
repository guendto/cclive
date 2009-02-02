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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <assert.h>

#include "llst.h"

int
llst_push (llst_node_t *head, const char *fmt, ...) {
    va_list args;
    char *str=0;

    assert(head != 0);
    assert(fmt != 0);

    va_start(args,fmt);
    vasprintf(&str,fmt,args);
    va_end(args);

    if (str) {
        struct _llst_node_s *n = malloc(sizeof(*n));
        if (n) {
            n->str  = str;
            n->next = *head;
            *head   = n;
            return(0);
        } else {
            free(str);
            perror("malloc");
        }
    }
    else
        perror("vasprintf");
    return(1);
}

int
llst_append (llst_node_t *head, const char *fmt, ...) {
    llst_node_t curr=*head;
    va_list args;
    char *str=0;
    int rc;

    assert(head != 0);
    assert(fmt != 0);

    va_start(args,fmt);
    vasprintf(&str,fmt,args);
    va_end(args);

    if (curr == 0)
        rc = llst_push(head,str);
    else {
        while (curr->next != 0)
            curr = curr->next;
        rc = llst_push(&(curr->next),str);
    }
    return(rc);
}

size_t
llst_size (llst_node_t head) {
    llst_node_t curr = head;
    size_t c = 0;
    assert(head != 0);
    while (curr != 0) {
        curr = curr->next;
        ++c;
    }
    return(c);
}

void
llst_dump (llst_node_t head) {
    llst_node_t curr = head;
    assert(head != 0);
    while (curr != 0) {
        puts(curr->str);
        curr = curr->next;
    }
}

void
llst_free (llst_node_t *head) {
    llst_node_t curr = *head;
    assert(head != 0);
    while (curr != 0) {
        free(curr->str);
        free(curr);
        curr=curr->next;
    }
    *head=0;
}

#ifdef TEST_LLST
int
main (int argc, char *argv[]) {
    llst_node_t head=0;
    int i;

    for (i=0; i<5; ++i)
        llst_push(&head,"item #%d",i+1);

    printf("size: %d\n",llst_size(head));
    llst_dump(head);
    llst_free(&head);
    printf("size: %d\n",llst_size(head));

    for (i=0; i<5; ++i)
        llst_append(&head,"item #%d",i+1);

    printf("size: %d\n",llst_size(head));
    llst_dump(head);
    llst_free(&head);
    printf("size: %d\n",llst_size(head));

    return(0);
}
#endif
