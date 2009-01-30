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
#ifndef progress_h
#define progress_h

struct ccprogress_s {
    time_t last_update;
    time_t started;
    double initial;
    double total;
    double count;
    char *fn;
    int done;
};

typedef struct ccprogress_s * ccprogress_t;

extern void
cc_bar_init(ccprogress_t bp, double initial, double total);

extern void
cc_bar_update(ccprogress_t bp, double total, double now);

extern void
cc_bar_finish(ccprogress_t bp);

extern int
cc_progress_cb(void *p, double total, double now,
    double utotal, double unow);

#endif
