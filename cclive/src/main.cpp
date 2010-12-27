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

#include <iostream>

#include "cclive/log.h"
#include "cclive/application.h"

int
main (int argc, char *argv[]) {

    cclive::application app;
    int rc = 0; // OK.

    try { rc = app.exec(argc,argv); }

    catch (const quvicpp::error& e)
        { cclive::log << "libquvi: error: " << e.what() << std::endl; }

    catch (const std::runtime_error& e)
        { cclive::log << "error: " << e.what() << std::endl; }

    return rc;
}


