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
#include <time.h>
#include <string.h>
#include <assert.h>

#include "cclive.h"
#include "progress.h"

static void /* convert time to formatted string */
time2str (int secs, char *dst, size_t size) {
    assert(dst != 0);
    assert(size > 0);
    if (secs < 100)
        snprintf(dst,size,"%02ds", secs);
    else if (secs < 100 * 60)
        snprintf(dst,size,"%02dm%02ds", secs/60, secs%60);
    else if (secs < 48 * 3600)
        snprintf(dst,size,"%02dh%02dm", secs/3600, (secs/60)%60);
    else if (secs < 100 * 86400)
        snprintf(dst,size,"%02dd%02dh", secs/86400, (secs/3600)%60);
    else
        snprintf(dst,size,"%02dd", secs/86400);
}

static void /* return transfer units */
get_units (double *rate, char **unit) {
    static const char *units[] = {"K/s", "M/s", "G/s"};
    int i=0;

    assert(rate != 0);
    assert(unit != 0);

    if (*rate < 1024*1024) {
        *rate /= 1024;
    } else if (*rate < 1024*1024) {
        *rate /= 1024*1024;
        i=1;
    } else if (*rate < 1024*1024*1024) {
        *rate /= 1024*1024*1024;
        i=2;
    }
    *unit = (char *)units[i];
}

void /* init progressbar */
bar_init (progressbar_t bp, double initial, double total) {
    assert(bp != 0);
    if (initial > total)
        total = initial;
    bp->initial = initial; /* bytes dl previously */
    bp->total   = total;   /* expected bytes */
    time(&bp->started);
}

#define BP_REFRESH_INTERVAL 0.2
#define BP_DEFAULT_WIDTH    80

void /* update progressbar */
bar_update (progressbar_t bp, double total, double now) {
    time_t tnow, elapsed;
    double size, rate;
    char buffer[80];
    char *p=(char*)&buffer;
    char tmp[30];
    int i,l;

    assert(bp != 0);

    *p='\0';

    time(&tnow);
    elapsed = tnow - bp->started;

    if (!bp->done) {
        if ( (elapsed - bp->last_update) < BP_REFRESH_INTERVAL)
            return;
    }
    else
        now = bp->total;

    bp->last_update = elapsed;
    size            = bp->initial + now;

    snprintf(tmp,sizeof(tmp),"%s",bp->fname);
    p += sprintf(p,tmp); /* max. +30 */

    if (bp->total > 0) {
        double _size = !bp->done ? size:now;
        int percent = 100.0 * size / bp->total;
        if (percent < 100)
            p += sprintf(p,"  %2d%% ",percent); /* +6 = 36 */
        else
            p += sprintf(p,"  100%%"); /* +6 = 36 */
        snprintf(tmp,20,
            "  %4.2fMB / %4.2fMB",ToMB(_size),ToMB(bp->total));
        p += sprintf(p,tmp); /* max. +20 = 56 */
    }

    rate = elapsed ? (now/elapsed) : 0;
    if (rate > 0) {
        char eta[12];
        char *unit = "";
        if (!bp->done) {
            int left = (int)((total-now)/rate);
            time2str(left,eta,sizeof(eta));
        } else {
            time2str(elapsed,eta,sizeof(eta));
        }
        get_units(&rate,&unit);
        snprintf(tmp,20,"  %4.2f%s  %6s",rate,unit,eta); /* max. +20 = 76 */
    } else {
        snprintf(tmp,20,"  --.-K/s  --:--");  /* +16 = 72 */
    }

    /* pad to max. width and append rate+eta */
    for (i=p-buffer,l=strlen(tmp); i<BP_DEFAULT_WIDTH-l; ++i)
        *p++ = ' ';
    p += sprintf(p,tmp);

    bp->count = now;
    cc_log("\r%s",buffer);
}

void /* tidy up after file transfer */
bar_finish (progressbar_t bp) {
    assert(bp != 0);
    if (bp->total > 0
        && bp->count + bp->initial > bp->total)
    {
        bp->total = bp->initial + bp->count;
    }
    bp->done = 1;
    bar_update(bp,-1,-1);
}

int /* curl progress callback function */
progress_cb (
    void *p,
    double total,
    double now,
    double utotal,
    double unow)
{
    assert(p != 0);
    bar_update(p,total,now);
    return(0);
}
