Experimental cclive w32 binaries.

* You will need to get (lib)quvi binaries from:
  http://quvi.googlecode.com/

* Copy the contents of quvi bin/ directory in the same directory
with the cclive binaries, make sure the lua/ directory is found
in the same directory with these binaries

* Optimized for i686 
* See cclive.1.html for the manual page
* Some options, e.g.--stream-exec, --background are not supported
* Config not supported unless CCLIVE_HOME is set
  - e.g. set CCLIVE_HOME c:\cclive)
* Config filename is ccliverc (not .ccliverc)
* cclive -v displays path to config file if it is found and used
