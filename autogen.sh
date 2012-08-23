#!/bin/sh
# autogen.sh for cclive.

source=.gitignore
cachedir=autom4te.cache

gen_manual()
{
  echo "Generate manual..."
  MAN=doc/man1/cclive.1 ; POD=$MAN.pod ; VN=`./gen-ver.sh`
  podchecker "$POD" || exit $?
  pod2man -c "cclive manual" -n cclive -s 1 -r "$VN" "$POD" "$MAN"
  return $?
}

cleanup()
{
  echo "WARNING
This will remove the files specified in the $source file. This will also
remove the $cachedir/ directory with all of its contents.
  Bail out now (^C) or hit enter to continue."
  read n1
  for file in `cat $source`; do # Remove files only.
    [ -e "$file" ] && [ -f "$file" ] && rm -f "$file"
  done
  [ -e "$cachedir" ] && rm -rf "$cachedir"
  rmdir -p config.aux 2>/dev/null
  exit 0
}

help()
{
  echo "Usage: $0 [-c|-h]
-h  Show this help and exit
-c  Make the source tree 'maintainer clean'
Run without options to (re)generate the configuration files."
  exit 0
}

clean_flag=off
while [ $# -gt 0 ]
do
  case "$1" in
    -c) cleanup;;
    -h) help;;
    *) break;;
  esac
  shift
done

echo "Generate configuration files..."
autoreconf -if \
    && gen_manual \
      && echo "You can now run 'configure'"
