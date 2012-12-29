AC_INIT([timer], [0.2], [sakhnik@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AM_PROG_CC_C_O

m4_include(ax_pthread.m4)
AX_PTHREAD(,AC_MSG_ERROR([Missing pthread support]))

AM_PATH_GTK_2_0([2.8.0])
PKG_CHECK_MODULES(GTK, gdk-2.0 gtk+-2.0 cairo,,exit)
AC_SUBST(GTK_LIBS)
AC_SUBST(GTK_CFLAGS)

dnl AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
