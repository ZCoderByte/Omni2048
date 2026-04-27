AC_INIT([Omni2048], [v0.3.1], [])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
PKG_PROG_PKG_CONFIG
AC_PROG_CC

AC_ARG_WITH([curses],
  [AS_HELP_STRING([--with-curses=TYPE],
    [Choose curses backend: auto, ncurses, pdcurses, or system])],
  [with_curses=$withval],
  [with_curses=no])

AS_CASE([$with_curses],
  [no|auto|ncurses|pdcurses|system], [],
  [AC_MSG_ERROR([invalid value for --with-curses: $with_curses])])

AM_CONDITIONAL([IO_CURSES], [test "x$with_curses" != "xno" -a "x$with_curses" != "x"])
# If you want a real boolean conditional instead, use a separate variable like use_curses=yes/no.

curses_backend=
CURSES_CFLAGS=
CURSES_LIBS=

AS_CASE([$with_curses],
  [auto], [
    case $host_os in
      mingw*|msys*|cygwin*)
        with_curses=pdcurses
        ;;
      *)
        with_curses=ncurses
        ;;
    esac
  ])

AS_CASE([$with_curses],
  [ncurses], [
    AX_WITH_CURSES
    AS_IF([test "x$ax_cv_curses" != xyes], [AC_MSG_ERROR([ncurses requested but not found])])
    curses_backend=ncurses
  ],
  [system], [
    AX_WITH_CURSES
    AS_IF([test "x$ax_cv_plaincurses" != xyes], [AC_MSG_ERROR([system curses requested but not found])])
    curses_backend=system
  ],
  [pdcurses], [
    PKG_CHECK_MODULES([CURSES], [pdcurses], [
      curses_backend=pdcurses
    ], [
      AC_CHECK_LIB([pdcurses], [initscr],
        [CURSES_LIBS="-lpdcurses"; curses_backend=pdcurses],
        [AC_MSG_ERROR([PDCurses requested but not found])])
    ])
  ])
if test "x$curses_backend" != x; then
  AC_DEFINE([IO_CURSES], [1], [Define if using a curses implementation as the I/O system.])
  AC_SUBST([CURSES_CFLAGS])
  AC_SUBST([CURSES_LIBS])
else
  AC_DEFINE([IO_STDIO], [1], [Define if using stdio functions as the I/O system.])
fi

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
