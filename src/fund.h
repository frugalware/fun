#ifndef _FUND_H
#define _FUND_H

typedef struct
{
	GObject parent;
	DBusGConnection *connection;
} UpdNotifier;

typedef struct
{
	GObjectClass parent_class;
} UpdNotifierClass;

static void updnotifierd_init(UpdNotifier *server);
static void updnotifierd_class_init(UpdNotifierClass *class);

gboolean updnotifier_update_database(UpdNotifier *obj, gchar **packages, GError **error);
gboolean updnotifier_test_service(UpdNotifier *obj, gint *ret, GError **error);

#endif
