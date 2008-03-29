INCLUDES =	@FUND_CFLAGS@ \
		   	@FUN_CFLAGS@ \
			-DPREFIX=\"$(prefix)\" \
			-fPIC

sbin_PROGRAMS = fund
bin_PROGRAMS = fun

fund_SOURCES = fund.c
fun_SOURCES	= wejpconfig.c \
			  fun-config.c \
			  fun-tooltip.c \
			  fun-messages.c \
			  fun-ui.c \
			  fun-news_backend.c \
			  fun-dbus.c \
			  fun.c

fund_LDADD= @FUND_LIBS@ -lpacman -lfwutil
fun_LDADD= @FUN_LIBS@ @FUND_LIBS@

BUILT_SOURCES = fund-dbus-glue.h

fund-dbus-glue.h: fund.xml
	@DBUSBINDINGTOOL@ --mode=glib-server --prefix=fund \
		$(srcdir)/fund.xml > fund-dbus-glue.h

EXTRA_DIST = fund.xml
DISTCLEANFILES = \
	fund-dbus-glue.h
