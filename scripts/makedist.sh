#!/bin/sh

project=cclive
branch=master
after="Thu Aug 12 15:54:58 2010"

descr=`git describe $branch`
basename="$project-$descr"
tarball="$basename.tar.gz"

echo ":: Generate $tarball"

# Note: we commit ChangeLog (temporarily) changes so that git-archive
# picks the generated changes. Perhaps there's a more convinient way.

git log --stat --after="$after" >ChangeLog && \
git commit -q ChangeLog -m "Generate $tarball" && \
git archive $branch --prefix="$basename/" | gzip >$tarball && \
git reset -q --hard HEAD~1
