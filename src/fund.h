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
gboolean fund_get_package_info(FWUpdateNotifier *obj, gchar *package, gchar **version, gchar **desc, GError **error);
gboolean fund_test_service(FWUpdateNotifier *obj, gint *ret, GError **error);

#endif
