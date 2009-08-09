/*
 * Copyright (C) 2009 Toni Gundogdu.
 *
 * This file is part of cclive.
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

#ifndef progressbar_h
#define progressbar_h

class ProgressBar {
public:
    ProgressBar();
public:
    void init   (const VideoProperties&);
    void update (double now);
    void finish ();
private:
    const std::string timeToStr    (const int& secs) const;
    const std::string getUnit      (double& rate) const;
    void              forkStreamer ();
private:
    VideoProperties props;
    time_t lastUpdate;
    time_t started;
    double initial;
    double total;
    double count;
    bool   done;
    int    width;
    int    termWidth;
    bool   streamFlag;
    pid_t  streamPid;
private:
    enum { DEFAULT_TERM_WIDTH=80 };
};

#endif
