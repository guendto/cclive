#!/bin/sh
#
# cclive
# Copyright (C) 2012  Toni Gundogdu <legatvs@gmail.com>
#
# This file is part of cclive <http://cclive.sourceforge.net/>.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public
# License along with this program.  If not, see
# <http://www.gnu.org/licenses/>.
#
set -e
IFS=$'
'
v=`./gen-ver.sh`
b="$v  [`date +%F`]

"
t=`git describe --abbrev=0`
for a in `git log --no-merges --format='%an' "$t"..HEAD | sort | uniq`; do
  c=`git log --no-merges --format="  - %s [%h]" --author="$a" "$t"..HEAD | sort -h`
  b="$b$a:
$c
"
done
b="$b
`git diff --shortstat "$t"..HEAD`

"
n=`cat NEWS`
F=NEWS
echo "$b"  >"$F" ; echo "$n" >>"$F"
