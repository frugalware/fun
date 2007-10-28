#ifndef _FUN_DBUS_H
#define _FUN_DBUS_H

#include <stdio.h>
#include <glib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

#define PERFORM_UPDATE	1
#define TEST_SERVICE	2

gboolean fun_dbus_init (void);

gboolean fun_dbus_perform_service (guint);

#endif
