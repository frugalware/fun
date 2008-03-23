ENV_CFLAGS=$CFLAGS

AC_PREREQ(2.59)
AC_INIT([fun], 0.2.2, [frugalware-devel@frugalware.org], fun)
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

AC_PATH_PROG(DBUSBINDINGTOOL, dbus-binding-tool, no)

if test "x$DBUSBINDINGTOOL" = "xno"; then
	AC_MSG_ERROR([dbus-binding-tool not found in your PATH. Please correct this])
fi

AC_SUBST(DBUS_BINDING_TOOL)
ui_modules="gtk+-2.0 >= 2.12.0 libglade-2.0 >= 2.6.0 libnotify"
PKG_CHECK_MODULES([FUND], [dbus-glib-1 >= 0.73])
PKG_CHECK_MODULES([FUN], [$ui_modules])

AC_ARG_WITH(dbus-sys, [  --with-dbus-sys=<dir>   where D-BUS system.d directory is])

if ! test -z "$with_dbus_sys" ; then
	DBUS_SYS_DIR="$with_dbus_sys"
else
	DBUS_SYS_DIR="$sysconfdir/dbus-1/system.d"
fi

AC_SUBST(DBUS_SYS_DIR)

CFLAGS="${CFLAGS}"

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
"
