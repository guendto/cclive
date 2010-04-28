
These w32 binaries are experimental. This package contains all of the
binaries required to run cclive on this platform. If you are planning
to run cclive system wide, see the "Important" section below.

Notes
-----
* These binaries have been optimized for i686
* cclive -v displays path to config file if it is found and used
* cclive(1) manual page has been converted to HTML (cclive.1.html)

Platform notes
--------------
* Some program features (e.g. --background, --stream-exec) have
  been disabled at compile time due to lack system call fork(3)
  which is not available for this platform

Important
---------
* Config not supported unless CCLIVE_HOME is set
  - e.g. set CCLIVE_HOME c:\cclive)
  - Note that the config is called "ccliverc" and not ".ccliverc"

* If you are running cclive outside the bin/ directory
  - set QUVI_BASEDIR=PATH_TO_BINDIR
  - e.g. set QUVI_BASEDIR=c:\cclive-0.6.3\bin
  - Otherwise libquvi will complain about not finding the lua scripts
