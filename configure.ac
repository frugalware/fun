ENV_CFLAGS=$CFLAGS

AC_PREREQ(2.59)
AC_INIT([fun], 0.3.3, [frugalware-devel@frugalware.org], fun)
AC_CONFIG_SRCDIR([src/fun.c])
AC_LANG(C)
AM_CONFIG_HEADER(config.h)
AC_CANONICAL_SYSTEM
AC_PROG_LIBTOOL
AC_PROG_INTLTOOL([0.21])
AM_INIT_AUTOMAKE

AC_PROG_CC
AC_HEADER_STDC
AC_CHECK_HEADERS([locale.h stdlib.h string.h sys/ioctl.h unistd.h pacman.h libfwutil.h])
ALL_LINGUAS=""
GETTEXT_PACKAGE=fun
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [needed for bindtextdomain() and textdomain()])
AM_GLIB_GNU_GETTEXT
AM_GLIB_DEFINE_LOCALEDIR(LOCALEDIR)

AC_ARG_ENABLE(werror,
			  AC_HELP_STRING([  --enable-werror], [Enable -werror
							  usage]), [werror="yes"])
AC_ARG_ENABLE(debug,
			  AC_HELP_STRING([  --enable-debug], [Enable debugging
							  support]), [debug="yes"])

dnl Check for -werror usage
AC_MSG_CHECKING(for werror usage)
if test "x$werror" = "xyes" ; then
	CFLAGS="${CFLAGS} -Werror"
	werror=yes
	AC_MSG_RESULT(yes)
else
	werror=no
	AC_MSG_RESULT(no)
fi

dnl Build with strict
AC_ARG_ENABLE(strict,
	AC_HELP_STRING([ --enable-strict],  [Enable building with 'Strict' flags]),
	[strict=$enableval], [strict=no])

dnl Enable or disable building with 'strict' flags
AC_MSG_CHECKING(for building with 'strict' flags)
if test x$strict = xyes ; then
	CFLAGS="$CFLAGS -Wextra -Wall -pedantic -Wshadow \
	    -Wmissing-declarations -Wpointer-arith -Wcast-align \
	    -Wfloat-equal -Wstrict-prototypes -Wmissing-prototypes \
	    -Wredundant-decls -Wunused-variable -Wunused-parameter \
	    -Wparentheses -Wno-long-long -Wwrite-strings -Wimplicit \
	    -Wnested-externs -Wno-unused -Wundef -Wpacked \
	    -Wbad-function-cast -Wunused-function -Wunused-label \
	    -Wunused-value -Wfloat-equal -Wchar-subscripts -Wswitch \
	    -Wmissing-braces -Wsequence-point -Wunknown-pragmas \
	    -Wdeclaration-after-statement -Wsign-compare"
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

dnl Check if debug is enabled
AC_MSG_CHECKING(for debug support)
if test "x$debug" = "xyes" ; then
	AC_DEFINE(FUN_DEBUG, 1, [Debug support: enabled])
	CFLAGS="${CFLAGS} -ggdb3 -Wall -DFUN_DEBUG"
	debug=yes
	AC_MSG_RESULT(yes)
else
	AC_DEFINE(FUN_DEBUG, 0, [Debug support: disabled])
	debug=no
	AC_MSG_RESULT(no)
fi

AC_SUBST([FUN_DEBUG])
AC_PATH_PROG(DBUSBINDINGTOOL, dbus-binding-tool, no)

if test "x$DBUSBINDINGTOOL" = "xno"; then
	AC_MSG_ERROR([dbus-binding-tool not found in your PATH. Please correct this])
fi

AC_SUBST(DBUS_BINDING_TOOL)
ui_modules="gtk+-2.0 >= 2.12.0 libglade-2.0 >= 2.6.0 gthread-2.0 libnotify nxml"
PKG_CHECK_MODULES([FUND], [dbus-glib-1 >= 0.73])
PKG_CHECK_MODULES([FUN], [$ui_modules])

AC_ARG_WITH(dbus-sys, [  --with-dbus-sys=<dir>   where D-BUS system.d directory is])

if ! test -z "$with_dbus_sys" ; then
	DBUS_SYS_DIR="$with_dbus_sys"
else
	DBUS_SYS_DIR="$sysconfdir/dbus-1/system.d"
fi

AC_SUBST(DBUS_SYS_DIR)

CFLAGS="${CFLAGS} -std=c99"

dnl ==========================================================================

AC_OUTPUT([
src/Makefile
data/Makefile
data/icons/Makefile
data/icons/16x16/Makefile
data/icons/24x24/Makefile
data/icons/32x32/Makefile
data/icons/48x48/Makefile
data/icons/128x128/Makefile
po/Makefile.in
Makefile
])

echo "
FUN $VERSION configuration summary:

        prefix                   : ${prefix}
        source code location     : ${srcdir}
        compiler                 : ${CC}
        compiler flags           : ${CFLAGS}
        debug support            : ${debug}
        werror support           : ${werror}
	strict flags             : ${strict}

"
