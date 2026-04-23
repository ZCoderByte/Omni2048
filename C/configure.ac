AC_INIT([Omni2048], [0.3], [])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
PKG_PROG_PKG_CONFIG
AC_PROG_CC

AC_ARG_ENABLE([io-curses],
  [AS_HELP_STRING([--enable-io-curses], [Use a curses implementation as the I/O system.])],
  [],
  [enable_io_curses=no])

AM_CONDITIONAL([IO_CURSES], [test "x$enable_io_curses" = "xyes"])

if test "x$enable_io_curses" = "xyes"; then
  AX_WITH_CURSES
  AS_IF([test "x$ax_cv_curses" != xyes], [
    AC_MSG_ERROR([a curses library is required])
  ])

  AC_DEFINE([IO_CURSES], [1], [Define if using a curses implementation as the I/O system.])
  
  AC_SUBST([CURSES_CFLAGS])
  AC_SUBST([CURSES_LIBS])
else
  AC_DEFINE([IO_STDIO], [1], [Define if using stdio functions as the I/O system.])
fi

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
