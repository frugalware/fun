INCLUDES =	@UPDNOTIFIERD_CFLAGS@ \
		   	@FUN_CFLAGS@ \
			-DPREFIX=\"$(prefix)\" \
			-fPIC

sbin_PROGRAMS = updnotifierd
bin_PROGRAMS = fun

updnotifierd_SOURCES = updnotifierd.c
fun_SOURCES	= eggtrayicon.c	\
			  sexy-tooltip.c \
			  fun-tooltip.c \
			  fun-ui.c \
			  fun-dbus.c \
			  fun.c

updnotifierd_LDADD= @UPDNOTIFIERD_LIBS@ -lpacman -lfwutil
fun_LDADD= @FUN_LIBS@ @UPDNOTIFIERD_LIBS@

BUILT_SOURCES = updnotifierd-dbus-glue.h

updnotifierd-dbus-glue.h: updnotifierd.xml
	@DBUSBINDINGTOOL@ --mode=glib-server --prefix=updnotifierd \
		$(srcdir)/updnotifierd.xml > updnotifierd-dbus-glue.h

EXTRA_DIST = updnotifierd.xml
DISTCLEANFILES = \
	updnotifierd-dbus-glue.h
