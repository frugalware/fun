#ifndef _FUND_H
#define _FUND_H

typedef struct
{
	GObject parent;
	DBusGConnection *connection;
} FWUpdateNotifier;

typedef struct
{
	GObjectClass parent_class;
} FWUpdateNotifierClass;

static void fund_init(FWUpdateNotifier *server);
static void fund_class_init(FWUpdateNotifierClass *class);

gboolean fund_update_database(FWUpdateNotifier *obj, gchar **packages, GError **error);
gboolean fund_get_package_version(FWUpdateNotifier *obj, gchar *package, gchar **version, GError **error);
gboolean fund_get_package_description(FWUpdateNotifier *obj, gchar *package, gchar **description, GError **error);
gboolean fund_test_service(FWUpdateNotifier *obj, gint *ret, GError **error);

#endif
