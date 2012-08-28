#!/bin/sh
#
# gen-news.sh for cclive
# Copyright (C) 2012  Toni Gundogdu
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
set -e
IFS=$'
'
a=`grep < ~/.gitconfig name | cut -f2 -d= | sed 's/^\s*//'`
v=`./gen-ver.sh`
d=`date +%F`
b="$v ($d) / $a

"
t=`git tag | tail -n1`
for a in `git log --format='%an' "$t"..HEAD | sort | uniq`; do
  c=`git log --format="  - %s [%h]" --author="$a" "$t"..HEAD | sort -h`
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
