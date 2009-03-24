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
#ifdef HAVE_UNISTD_H
  #include <unistd.h>
#endif
#include <sys/wait.h>
#include <time.h>
#ifdef HAVE_STRING_H
  #include <string.h>
#endif
#ifdef WITH_SIGWINCH
  #include <signal.h>
  #ifdef HAVE_SYS_IOCTL_H
    #include <sys/ioctl.h>
  #endif
#endif
#ifdef HAVE_SYS_TYPES_H
  #include <sys/types.h>
#endif
#include <assert.h>

#include "cclive.h"
#include "progress.h"

static int streaming_flag;
static pid_t streaming_pid;

static int term_width;
#ifdef WITH_SIGWINCH
static volatile sig_atomic_t recv_sigwinch;

RETSIGTYPE
handle_sigwinch (int sig) {
    recv_sigwinch = 1;
    signal(SIGWINCH, handle_sigwinch);
}

static int
get_term_width (void) {
    int fd = fileno(stderr);
    struct winsize wsz;

    if (ioctl(fd,TIOCGWINSZ,&wsz) < 0)
        return(0);

    return(wsz.ws_col);
}
#endif

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

static void
fork_streamer (progressbar_t bp) {
    streaming_flag = 1;
#if defined(HAVE_FORK) && defined(HAVE_WORKING_FORK)
    if ( (streaming_pid = fork()) < 0) {
        perror("fork");
    } else if (streaming_pid == 0) {
        char *cmd = strrepl(cc.gi.stream_exec_arg,"%i",bp->fname);
        if (cmd)
            system(cmd);
        FREE(cmd);
        exit(0);
    }
#endif
}

#define DEFAULT_TERM_WIDTH  80

void /* init progressbar */
bar_init (progressbar_t bp, double initial, double total) {
    assert(bp != 0);
    streaming_flag = 0;
    if (initial > total)
        total = initial;
    bp->initial = initial; /* bytes dl previously */
    bp->total   = total;   /* expected bytes */
#ifdef WITH_SIGWINCH
    if (!term_width || recv_sigwinch) {
        term_width = get_term_width();
        if (!term_width)
            term_width = DEFAULT_TERM_WIDTH;
    }
#else
    term_width = DEFAULT_TERM_WIDTH;
#endif
    bp->width   = term_width-1; /* do not use the last column */
    time(&bp->started);
}

#define REFRESH_INTERVAL 0.2

void /* update progressbar */
bar_update (progressbar_t bp, double total, double now) {
    static char buffer[1024];
    char *p = (char *)buffer;
    int force_update = 0;
    time_t tnow,elapsed;
    double size,rate;
    char tmp[1024];
    int i,l;

    assert(bp != 0);
#ifdef WITH_SIGWINCH
    if (recv_sigwinch) {
        int old_width = term_width;
        term_width    = get_term_width();
        if (!term_width)
            term_width = DEFAULT_TERM_WIDTH;
        if (term_width != old_width) {
            bp->width    = term_width - 1;
            force_update = 1;
        }
        recv_sigwinch = 0;
    }
#endif
    time(&tnow);
    elapsed = tnow - bp->started;

    if (!bp->done) {
        if ( (elapsed - bp->last_update) < REFRESH_INTERVAL && !force_update)
            return;
    }
    else
        now = bp->total;

    bp->last_update = elapsed;
    size            = bp->initial + now;

    l = 32;
    if (bp->width > DEFAULT_TERM_WIDTH) {
        l += bp->width - DEFAULT_TERM_WIDTH;
        if (l > sizeof(tmp))
            l = sizeof(tmp);
    }
    snprintf(tmp,l,"%s",bp->fname);
    p += sprintf(p,"%s",tmp);

    if (bp->total > 0) {
        double _size = !bp->done ? size:now;
        int percent = 100.0 * size / bp->total;
        if (percent < 100)
            p += sprintf(p,"  %2d%% ",percent); /* +6 = 38 */
        else
            p += sprintf(p,"  100%%"); /* +6 = 38 */
        snprintf(tmp,20,
            "  %4.1fM / %4.1fM",ToMB(_size),ToMB(bp->total));
        p += sprintf(p,"%s",tmp); /* max. +20 = 58 */

        if (cc.gi.stream_given
            && cc.gi.stream_exec_given
            && !streaming_flag)
        {
            if (percent >= cc.gi.stream_arg)
                fork_streamer(bp);
        }
    }

    rate = elapsed ? (now/elapsed) : 0;
    if (rate > 0) {
        char *unit = "";
        char eta[12];
        if (!bp->done) {
            int left = (int)((total-now)/rate);
            time2str(left,eta,sizeof(eta));
        }
        else
            time2str(elapsed,eta,sizeof(eta));
        get_units(&rate,&unit);
        snprintf(tmp,20,"  %4.1f%s  %6s",rate,unit,eta); /* max. +20 = 78 */
    }
    else
        sprintf(tmp,"  --.-K/s  --:--");  /* +16 = 74 */

    /* pad to max. width leaving enough space for rate+eta */
    l = bp->width - strlen(tmp);
    i = p - buffer;
    while (i<l) {
        *p++ = ' ';
        ++i;
    }
    p += sprintf(p,"%s",tmp);
    *p='\0';

    cc_log("\r%s",buffer);
    bp->count = now;
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

    if (streaming_flag) {
        if (waitpid(streaming_pid,0,0) != streaming_pid)
            perror("waitpid");
    }
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
