/*
 * cclive Copyright (C) 2009 Toni Gundogdu. This file is part of cclive.
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

#ifndef singleton_h
#define singleton_h

#include "config.h"

#ifndef HAVE_PTRDIFF_T
#error Cannot compile without ptrdiff_t support
#endif

#include <cassert>

template <typename T> class Singleton {
private:
    static T* instance;
public:
    Singleton() {
        assert(!instance);
        ptrdiff_t offset = (ptrdiff_t)(T*)1 - (ptrdiff_t)(Singleton <T>*)(T*)1;
        instance = (T*)(this+offset);
    }
    virtual ~Singleton() {
        assert(instance != 0);
        instance = 0;
    }
public:
    static T& getInstance() {
        assert(instance != 0);
        return *instance;
    }
};

template <typename T> T* Singleton<T>::instance = 0;

#endif
