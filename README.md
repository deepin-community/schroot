# reschroot

Lightweight virtualisation tool

This is the reboot of the `schroot` tool conceived and developed for
many years by Roger Leigh <rleigh@codelibre.net>.

We wish to thank Roger for all the efforts spent on this, we appreciate
it.

  Christoph Biedl <debian.axhn@manchmal.in-ulm.de>
  Jakob Haufe <sur5r@debian.org>


## Branch name convention

* "main" is the current development. It is in continuation of the old
  "schroot-1.6" branch.
* "schroot-1.6" is the schroot stable development. Not about to change.
* "schroot-1.7" was called "master" in schroot, is was the development
   branch. Not about to change.


## Contact

The preferred way to report bugs is the Debian bug tracker, by sending
an e-mail with some additional pseudo-headers, or (recommended) using
the reportbug(1) program. See <https://www.debian.org/Bugs/Reporting>,
file your bug report against the schroot package.


Original README follows below:

```

schroot
=======

Securely enter a chroot and run a command or login shell.

Note that giving untrusted users root access to chroots is a serious
security risk!  Although the untrusted user will only have root access
to files inside the chroot, in practice there are many obvious ways of
breaking out of the chroot and of disrupting services on the host
system.  As always, this boils down to trust.  Don't give chroot root
access to users you would not trust with root access to the host
system.

For compatibility with existing tools and scripts, wrapper binaries
for dchroot and DSA dchroot are provided.


Build Dependencies
------------------

An ISO C++ compiler supporting TR1 (e.g. GCC 4.x), or an ISO C++
compiler and the Boost libraries
libstdc++ (GNU libstdc++)
libpam0g-dev (Linux-PAM)
libboost-dev                 }
libboost-program-options-dev } The Boost C++ libraries
libboost-regex-dev           }
uuid-dev (e2fsprogs)
groff (or troff) soelim
cmake (required unless using the autotools configure script)

If building from GIT, you will also need:
gettext (0.16 or greater)
doxygen
po4a (>=0.40)

To build with cmake:

  cmake /path/to/schroot
  make

Or with autotools configure:

  /path/to/schroot/configure
  make


Translation
-----------

If you would like to see the schroot messages output in your own
language, please consider translating the pot file (po/schroot.pot).
If you would like to see the schroot man pages in your own language,
please consider translating the pot file
(man/po/schroot-man.pot).


Building and installation
-------------------------

cmake
^^^^^

Run "cmake -LH" to see basic configurable options.  The following basic
options are supported:

  btrfs-snapshot=(ON|OFF)  Enable support for btrfs snapshots (requires Btrfs)
  dchroot=(ON|OFF)         Enable dchroot compatibility
  dchroot-dsa=(ON|OFF)     Enable dchroot-dsa compatibility
  debug=(ON|OFF)           Enable debugging messages
  default_environment_filter=REGEX
                           Default environment filter
  doxygen=(ON|OFF)         Enable doxygen documentation
  loopback=(ON|OFF)        Enable support for loopback mounts
  lvm-snapshot=(ON|OFF)    Enable support for LVM snapshots (requires LVM)
  nls=(ON|OFF)             Enable national language support (requires gettext)
  pam=(ON|PFF)             Enable support for PAM authentication (requires libpam)
  personality=(ON|OFF)     Enable personality support (Linux only)
  test=(ON|OFF)            Enable unit tests
  union=(ON|OFF)           Enable support for union mounts
  uuid=(ON|OFF)            Enable support for UUIDs in session names (requires libuuid)

cmake will autodetect and enable all available features by default,
with the exception of dchroot and dchroot-dsa which require manually
specifying, so these options are mostly useful for disabling features
which are not required.

Run "cmake -LA" to see all settable options.  CMAKE_INSTALL_PREFIX is
the equivalent of the configure --prefix option.  Additionally,
CMAKE_INSTALL_SYSCONFDIR, CMAKE_INSTALL_LOCALSTATEDIR,
CMAKE_INSTALL_LIBDIR etc. provide the equivalent sysconfdir,
localstatedir and libdir, etc. options.

Run "make doc" to make the doxygen documentation.
Run "make test" to run the testsuite.

Note that the testsuite ("make test") should be run under fakeroot or
real root in order to work correctly.


autotools configure
^^^^^^^^^^^^^^^^^^^

Please see the INSTALL file for generic autotools configure
installation instructions.  There are the following additional
options:

  --enable-dchroot        Build dchroot for backward compatibility
  --enable-dchroot-dsa    Build DSA dchroot for backward compatibility
  --enable-debug          Enable debugging features
                          (not recommended--use --debug at runtime instead)
  --enable-environment-filter
                          Enable default environment filtering (regex)
  --enable-pam            Enable support for PAM authentication
  --enable-block-device   Enable support for block devices
  --enable-lvm-snapshot   Enable support for LVM snapshots
  --enable-btrfs-snapshot Enable support for btrfs snapshots
  --enable-loopback       Enable support for loopback mounts
  --enable-union          Enable support for union mounts
  --enable-doxygen        Enable doxygen documentation

Normally configure will autodetect and enable the recommended options
if supported by your system, so the above options should not usually
be required.

Run "make doc" to make the doxygen documentation.
Run "make check" to run the testsuite.

Note that the testsuite ("make check") should be run under fakeroot or
real root in order to work correctly.

Configuration
-------------

See schroot.conf(5) and schroot-setup(5).


Running
-------

See schroot(1).

```
