#!/bin/sh

# This script will commit ChangeLog temporarily so that git-archive
# picks the updated ChangeLog to the tarball.

project=cclive
branch=master
after="Thu Aug 12 15:54:58 2010"

echo "WARNING!

Please note that this script will call git-reset (--hard) during
this process so be sure to commit your changes before you run this
script.

Last chance to bail out (CTRL-C) before we call 'git reset --hard'."
read -s -n1

descr=`git describe $branch`
basename="$project-$descr"
tarball="$basename.tar.gz"

echo ":: Generate $tarball"

# TODO: Elegant way.
git log --stat --after="$after" >ChangeLog && \
git commit -q ChangeLog -m "Generate $tarball" && \
git archive $branch --prefix="$basename/" | gzip >$tarball && \
git reset -q --hard HEAD~1
