AC_PREREQ(2.59)
AC_INIT
AC_CONFIG_SRCDIR([eio.h])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE(libeio,1.0)
AM_MAINTAINER_MODE

AC_GNU_SOURCE

AC_PROG_LIBTOOL

AC_PROG_CC

if test "x$GCC" = xyes ; then
  CFLAGS="-O3 $CFLAGS"
fi

m4_include([libeio.m4])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
