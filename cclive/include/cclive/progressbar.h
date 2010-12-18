/* 
* Copyright (C) 2010 Toni Gundogdu.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef cclive_progressbar_h
#define cclive_progressbar_h

#include "cclive/file.h"

namespace cclive {

class progressbar {
    enum { default_term_width=80 };
public:
    enum mode { normal = 0, dotline };
public:
    progressbar (const file&, const quvicpp::link&, const options&);
    void update (double);
    void finish ();
private:

    void _normal (
        const std::stringstream& size_s,
        const std::stringstream& rate_s,
        const std::stringstream& eta_s,
        const std::stringstream& percent_s,
        const std::string& fname);

    void _dotline (
        const std::stringstream& size_s,
        const std::stringstream& rate_s,
        const std::stringstream& eta_s,
        const std::stringstream& percent_s,
        const std::string& fname);

private:
    int     _update_interval;
    double  _expected_bytes;
    double  _initial_bytes;
    time_t  _time_started;
    time_t  _last_update;
    size_t  _term_width;
    int     _dot_count;
    size_t  _old_width;
    double  _count;
    size_t  _width;
    file    _file;
    bool    _done;
    mode    _mode;
};

} // End namespace.

#endif



