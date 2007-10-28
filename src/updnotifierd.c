/*
 *  updatenotifierd.c for updatenotifier
 *
 *  Copyright (C) 2007 by Priyank Gosalia <priyankmg@gmail.com>
 *  Portions of this code are borrowed fron netconfigd
 *  netconfigd is Copyright (C) 2007 Alex Smith <alex@alex-smith.me.uk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <dbus/dbus-glib-bindings.h>
#include <libfwutil.h>
#include <dirent.h>
#include <config.h>
#include <syslog.h>
#include <pacman.h>
#include "updnotifierd.h"
#include "updnotifierd-dbus-glue.h"

typedef void* netbuf;
static PM_DB *sync_db = NULL;
static PM_DB *local_db = NULL;

G_DEFINE_TYPE(UpdNotifier, updnotifierd, G_TYPE_OBJECT);

void updnotifierd_class_init(UpdNotifierClass *class) {
	// Nothing here
}

void updnotifierd_init(UpdNotifier *server) {
	GError *error = NULL;
	DBusGProxy *driver_proxy;
	int request_ret;
	
	// Init the DBus connection
	server->connection = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
	if (server->connection == NULL) {
		g_warning("Unable to connect to dbus: %s", error->message);
		g_error_free(error);
		return;
	}
	
	dbus_g_object_type_install_info(updnotifierd_get_type(), &dbus_glib_updnotifierd_object_info);
	
	// Register DBUS path
	dbus_g_connection_register_g_object(server->connection, "/org/frugalware/UpdNotifier", G_OBJECT(server));

	// Register the service name, the constant here are defined in dbus-glib-bindings.h
	driver_proxy = dbus_g_proxy_new_for_name(server->connection, DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);

	if (!org_freedesktop_DBus_request_name (driver_proxy, "org.frugalware.UpdNotifier", 0, &request_ret, &error)) {
		g_warning("Unable to register service: %s", error->message);
		g_error_free(error);
	}
	
	g_object_unref(driver_proxy);
}

static void
_log_cb (unsigned short level, char *msg) {
	g_print ("%s\n", msg);

	return;
}

static gboolean
_updatenotifierd_init_pacman () {
	/* initialize the pacman-g2 library */
	if (pacman_initialize ("/") == -1)
		return FALSE;

	/* register the main repo */
	/* FIXME: Later add support for custom repos */
	sync_db = pacman_db_register ("frugalware-current");
	local_db = pacman_db_register ("local");

	if (sync_db == NULL)
		return FALSE;
	if (local_db == NULL)
		return FALSE;

	/* parse the pacman-g2 config */
	pacman_parse_config ("/etc/pacman.conf", NULL, "");
	pacman_set_option (PM_OPT_LOGMASK, (long)-1);
	pacman_set_option (PM_OPT_LOGCB, (long)_log_cb);
	
	return TRUE;
}

GList* _updnotifierd_update_database (void) {
	PM_LIST	*packages = NULL;
	GList	*ret = NULL;
	int	retval = 0;

	/* update the pacman database */
	retval = pacman_db_update (0, sync_db);
	
	/* something went wrong */
	if (retval== -1) {
		printf ("%s\n", pacman_strerror(pm_errno));
		return ret;
	}

	if (pacman_trans_init(PM_TRANS_TYPE_SYNC, 0, NULL, NULL, NULL) == -1) {
		gchar *errorstr = g_strdup_printf ("Failed to init transaction (%s)\n", pacman_strerror(pm_errno));
		g_print (errorstr);
		g_free (errorstr);
		return ret;
	}
	else {
		if (pacman_trans_sysupgrade() == -1) {
			g_print (pacman_strerror(pm_errno));
		}
		else {
			packages = pacman_trans_getinfo (PM_TRANS_PACKAGES);
			if (packages == NULL) {
				g_print ("No new updates are available\n");
			}
			else {
				PM_LIST *i = NULL;
	
				for (i=pacman_list_first(packages);i!=NULL;i=pacman_list_next(i)) {
					PM_SYNCPKG *spkg = pacman_list_getdata (i);
					PM_PKG *pkg = pacman_sync_getinfo (spkg, PM_SYNC_PKG);
					ret = g_list_append (ret, g_strdup((char*)pacman_pkg_getinfo(pkg,PM_PKG_NAME)));
				}
			}
			pacman_trans_release ();
		}
	}

	return ret;
}

gboolean updnotifier_update_database(UpdNotifier *obj, gchar **packages, GError **error) {
	GList *list = NULL;
	if ((list = _updnotifierd_update_database())==NULL) {
		*packages = NULL;
		return FALSE;
	}
	else {
		GString *tmp = g_string_new ("");
		for (list; list!=NULL;list=g_list_next(list)) {
			tmp = g_string_append (tmp, list->data);
			tmp = g_string_append (tmp, " ");
		}
		*packages = g_strdup (tmp->str);
		return TRUE;
	}
}

gboolean updnotifier_test_service(UpdNotifier *obj, gint *ret, GError **error) {
	*ret = 1;
	return TRUE;	
}

void usage() {
	printf("Fund v" VERSION "\n");
	printf(" --help        Display this help text\n");
	printf(" --daemon      Fork into the background\n");
}

int main (int argc, char *argv[]) {
	GMainLoop *main_loop;
	UpdNotifier *server;
	int i = 1;
	int daemonize = 0;
	
	// Parse command line options
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--daemon"))
			daemonize = 1;
		else if (!strcmp(argv[i], "--help")) {
			usage();
			return 0;
		} else {
			usage();
			return 1;
		}
	}
	
	if (getuid() != 0) {
		printf("Must run as root!\n");
		exit(1);
	}
	
	// Daemonize if wanted
	if (daemonize) {
		switch (fork()) {
			case 0:
				break;
			case -1:
				printf("Can't fork: %s\n", strerror(errno));
				exit(1);
				break;
			default:
				exit(0);
		}
		
		fclose(stdin);
		fclose(stdout);
		fclose(stderr);
	}
	
	// Connect to syslog
	openlog("updnotifierd", LOG_PID, LOG_DAEMON);
	
	syslog(LOG_INFO, "Fund v" VERSION " started...");
	
	g_type_init();
	
	server = g_object_new(updnotifierd_get_type(), NULL);

	_updatenotifierd_init_pacman ();
	main_loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(main_loop);
	
	return 0;
}

