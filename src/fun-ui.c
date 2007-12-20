/*
 *  fun-ui.c for fun
 *
 *  Copyright (C) 2007 by Priyank Gosalia <priyankmg@gmail.com>
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

#include "fun.h"
#include "fun-config.h"
#include "fun-messages.h"
#include "fun-tooltip.h"
#include "fun-dbus.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

static void fun_about_show (void);
static void fun_about_hide (void);
static void fun_main_window_init (void);
static void fun_main_window_show (void);
static void fun_main_window_hide (void);
static void fun_launch_gfpm (void);
static void fun_populate_updates_tvw (gchar *plist);

extern GladeXML *xml;

static GtkStatusIcon	*icon = NULL;

static GtkWidget		*fun_about_dlg = NULL;
static GtkWidget		*fun_main_window = NULL;
static GtkWidget		*fun_statusbar = NULL;
static GtkWidget		*fun_updates_tvw = NULL;
static GtkWidget		*fun_check_btn = NULL;
static GdkPixbuf		*fun_about_pixbuf = NULL;
static GtkWidget		*fun_config_dlg = NULL;
static GtkWidget		*fun_config_gfpm_launcher_combo = NULL;
static GtkAdjustment 	*fun_config_upd_int_adj = NULL;
static GtkAdjustment 	*fun_config_not_tim_adj = NULL;
static gboolean			connected = FALSE;

/* credits */
static const gchar *authors[] = { \
					"Priyank M. Gosalia <priyankmg@gmail.com>",
					NULL
				};

static const gchar *artists[] = { \
					"Viktor Gondor <nadfoka@frugalware.org>",
					"Priyank Gosalia <priyankmg@gmail.com>",
					NULL
				};

static const gchar translators[] = \
				"Carl Andersen <carl@frugalware.dk> (da_DK)\n"
				"Manuel Peral <mcklaren@gmail.com> (es_ES)\n"	
				"Michel Hermier <michel.hermier@gmail.com> (fr_FR)\n"
				"Miklos Vajna <vmiklos@frugalware.org> (hu_HU)\n";
				
static gchar *license =
("This program is free software; you can redistribute it and/or "
"modify it under the terms of the GNU General Public Licence as "
"published by the Free Software Foundation; either version 2 of the "
"Licence, or (at your option) any later version.\n"
"\n"
"This program is distributed in the hope that it will be useful, "
"but WITHOUT ANY WARRANTY; without even the implied warranty of "
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU "
"General Public Licence for more details.\n"
"\n"
"You should have received a copy of the GNU General Public Licence "
"along with this program; if not, write to the Free Software "
"Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, "
"MA  02110-1301  USA");


static gboolean fun_timeout_func (void);
static gboolean fun_timeout_conn (void);

static void fun_config_dialog_show (void);
static void fun_restart (void);
static GdkPixbuf * fun_get_icon (const char *icon, int size);
static void fun_update_status (const char *message);

static gboolean	cb_fun_systray_icon_clicked (GtkStatusIcon *widget, guint button, guint activate_time, gpointer data);
static void cb_fun_config_dlg_close_clicked (GtkWidget *button, gpointer data);

void
fun_systray_create (void)
{
	/* create the tray icon */
	icon = gtk_status_icon_new_from_icon_name ("fun");
	
	/* set the default tooltip */
	fun_tooltip_new (icon);
	fun_tooltip_set_text ("Frugalware Update Notifier", NULL);
	
	g_signal_connect (icon, "activate", G_CALLBACK (fun_main_window_show), NULL);
	g_signal_connect (icon, "popup-menu", G_CALLBACK (cb_fun_systray_icon_clicked), NULL);

	return;
}

static gboolean
cb_fun_systray_icon_clicked (GtkStatusIcon *widget, guint button, guint activate_time, gpointer data)
{
	/* Right Click */
	if (button == 3)
	{
		GtkWidget	*menu = NULL;
		GtkWidget	*menu_item = NULL;
		GtkWidget	*image = NULL;

		menu = gtk_menu_new ();

		/* About */
		menu_item = gtk_image_menu_item_new_with_label ("About FUN");
		image = gtk_image_new_from_stock ("gtk-about", GTK_ICON_SIZE_MENU);
		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(menu_item), image);
		g_signal_connect (G_OBJECT(menu_item), "activate", G_CALLBACK(fun_about_show), NULL);
		gtk_menu_shell_append (GTK_MENU_SHELL(menu), menu_item);
		gtk_widget_show (menu_item);

		/* Separator */
		menu_item = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
		gtk_widget_show (menu_item);

		/* Preferences */
		menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_PREFERENCES, NULL);
		g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (fun_config_dialog_show), NULL);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
		gtk_widget_show (menu_item);
		
		/* Quit */
		menu_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, NULL);
		g_signal_connect (G_OBJECT (menu_item), "activate", G_CALLBACK (gtk_main_quit), NULL);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
		gtk_widget_show (menu_item);

		gtk_widget_show (menu);
		gtk_menu_popup (GTK_MENU(menu),
				NULL,
				NULL,
				NULL,
				NULL,
				3,
				gtk_get_current_event_time());

		return;
	}
}

static void
cb_fun_config_dlg_close_clicked (GtkWidget *button, gpointer data)
{
	GtkAdjustment 	*adj = NULL;
	guint			interval = 0;
	guint			old_interval = 0;
	gint			sel = 0;
	
	/* gfpm_launcher setting */
	switch (gtk_combo_box_get_active(GTK_COMBO_BOX(fun_config_gfpm_launcher_combo)))
	{
		case 0:	/* gksu */
				fun_config_set_value_string ("gfpm_launcher", "gksu");
				break;
		case 1:	/* kdesu */
				fun_config_set_value_string ("gfpm_launcher", "kdesu");
				break;
		case 2:	/* sudo */
				fun_config_set_value_string ("gfpm_launcher", "sudo");
				break;
		default:
				break;
	}
	
	/* notification_timeout setting */
	sel = gtk_adjustment_get_value (fun_config_not_tim_adj);
	fun_tooltip_set_notification_timeout (sel);
	fun_config_set_value_int ("notification_timeout", sel);
	fun_config_save ();
	
	/* update_interval setting */
	adj = gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON(data));
	interval = gtk_adjustment_get_value (adj);
	old_interval = fun_config_get_value_int ("update_interval");
	/* if interval is changed, save the new interval value to .funrc and prompt
	 * the user to restart fun */
	/* TODO: implement a mechanism that automatically sets the timeout value to the 
	 * updated interval value without requiring to restart fun */
	if (old_interval != interval)
	{
		fun_config_set_value_int ("update_interval", interval);
		fun_config_save ();
		if (fun_question(_("Restart Fun"),
						_("Fun needs to be restarted in order for the changes to take effect. Do you want to restart Fun now ?")) == GTK_RESPONSE_YES)
		{
			fun_restart ();
		}
	}
	
	fun_config_save ();
	gtk_widget_hide (fun_config_dlg);
	
	return;
}

void
fun_ui_cleanup (void)
{
	if (icon == NULL)
		return;
	g_object_unref (icon);
	gtk_widget_destroy (GTK_WIDGET(fun_config_dlg));
	gtk_widget_destroy (GTK_WIDGET(fun_main_window));
	fun_tooltip_destroy ();
	icon = NULL;
	
	return;
}

static void
fun_restart (void)
{
	fun_ui_cleanup ();
	sleep (1);
	while (gtk_events_pending()) gtk_main_iteration ();
	system ("/usr/bin/fun");
	exit (0);

	return;
}

static void
fun_config_dialog_show (void)
{
	if (!GTK_WIDGET_VISIBLE(fun_config_dlg))
	{
		gtk_adjustment_set_value (fun_config_upd_int_adj, fun_config_get_value_int("update_interval"));
		gtk_adjustment_set_value (fun_config_not_tim_adj, fun_config_get_value_int("notification_timeout"));
		char *gfpm_launcher = fun_config_get_value_string("gfpm_launcher");
		if (!(strcmp(gfpm_launcher, "sudo")))
			gtk_combo_box_set_active (GTK_COMBO_BOX(fun_config_gfpm_launcher_combo), 2);
		else
		if (!(strcmp(gfpm_launcher, "gksu")))
			gtk_combo_box_set_active (GTK_COMBO_BOX(fun_config_gfpm_launcher_combo), 0);
		else
		if (!(strcmp(gfpm_launcher, "kdesu")))
			gtk_combo_box_set_active (GTK_COMBO_BOX(fun_config_gfpm_launcher_combo), 1);
		else
			gtk_combo_box_set_active (GTK_COMBO_BOX(fun_config_gfpm_launcher_combo), -1);
		gtk_widget_show (fun_config_dlg);
		gtk_window_present (GTK_WINDOW(fun_config_dlg));
	}
	else
	{
		gtk_window_present (GTK_WINDOW(fun_config_dlg));
	}
	
	return;
}

static void
fun_config_dialog_init (void)
{
	fun_config_dlg = glade_xml_get_widget (xml, "fun_config_dlg");
	fun_config_gfpm_launcher_combo = glade_xml_get_widget (xml, "fun_config_su");
	fun_config_upd_int_adj = gtk_spin_button_get_adjustment (glade_xml_get_widget(xml,"interval_spbtn"));
	fun_config_not_tim_adj = gtk_spin_button_get_adjustment (glade_xml_get_widget(xml,"notification_time_spbtn"));
	g_signal_connect (G_OBJECT(glade_xml_get_widget(xml,"pref_closebtn")),
					"clicked",
					G_CALLBACK(cb_fun_config_dlg_close_clicked),
					(gpointer)glade_xml_get_widget(xml,"interval_spbtn"));
	
	return;
}

static void
fun_main_window_init (void)
{
	GtkListStore		*store = NULL;
	GtkCellRenderer		*renderer = NULL;
	GtkTreeViewColumn	*column = NULL;
	
	fun_main_window = glade_xml_get_widget (xml, "fun_mainwindow");
	fun_statusbar = glade_xml_get_widget (xml, "fun_statusbar");
	fun_check_btn = glade_xml_get_widget (xml, "button_check");
	fun_updates_tvw = glade_xml_get_widget (xml, "update_list_tvw");
	store = gtk_list_store_new (4,
				GDK_TYPE_PIXBUF, 	/* Status icon */
				G_TYPE_STRING,   	/* Package name */
				G_TYPE_STRING,  	/* Latest version */
				G_TYPE_STRING);  	/* Package Description */
	renderer = gtk_cell_renderer_pixbuf_new ();
	column = gtk_tree_view_column_new_with_attributes (_("S"),
							renderer,
							"pixbuf", 0,
							NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(fun_updates_tvw), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Package Name"),
							renderer,
							"text", 1,
							NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_column_set_expand (column, FALSE);
	gtk_tree_view_column_set_min_width (column, 80);
	gtk_tree_view_append_column (GTK_TREE_VIEW(fun_updates_tvw), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Latest Version"),
							renderer,
							"text", 2,
							NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(fun_updates_tvw), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Description"),
							renderer,
							"text", 3,
							NULL);
	gtk_tree_view_column_set_min_width (column, 140);
	gtk_tree_view_column_set_expand (column, TRUE);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(fun_updates_tvw), column);

	gtk_tree_view_set_model (GTK_TREE_VIEW(fun_updates_tvw), GTK_TREE_MODEL(store));
	
	/* connect some important signals */
	g_signal_connect (G_OBJECT(glade_xml_get_widget(xml,"button_check")),
						"clicked",
						G_CALLBACK(fun_timeout_func),
						NULL);
	g_signal_connect (G_OBJECT(glade_xml_get_widget(xml,"button_about")),
						"clicked",
						G_CALLBACK(fun_about_show),
						NULL);
	g_signal_connect (G_OBJECT(glade_xml_get_widget(xml,"button_prefs")),
						"clicked",
						G_CALLBACK(fun_config_dialog_show),
						NULL);
	g_signal_connect (G_OBJECT(glade_xml_get_widget(xml,"button_close")),
						"clicked",
						G_CALLBACK(fun_main_window_hide),
						NULL);
	g_signal_connect (G_OBJECT(glade_xml_get_widget(xml,"button_main_close")),
						"clicked",
						G_CALLBACK(fun_main_window_hide),
						NULL);
	g_signal_connect (G_OBJECT(glade_xml_get_widget(xml,"launch_button")),
						"clicked",
						G_CALLBACK(fun_launch_gfpm),
						NULL);
					
	return;
}

void
fun_ui_init (void)
{
	GError		*error = NULL;
	gulong		seconds = 0;
	gchar		*plist = NULL;
	static gchar *error_msg = ("Update checking has been disabled because FUN has detected "
								"that the update notifier daemon is not running. FUN will attempt "
								"to reconnect to the daemon every 45 seconds. \n\nYou can start the "
								"update notifier daemon by running the following command as root: \n\n"
								"'service fun start'");
	fun_systray_create ();
	fun_main_window_init ();
	fun_config_dialog_init ();
	
	if (fun_dbus_perform_service (TEST_SERVICE, NULL, NULL, NULL) == FALSE)
	{
		g_error (_("Failed to connect to the fun daemon\n"));
		//fun_tooltip_set_text2 (tooltip, _("Not connected to fun daemon"), FALSE);
		connected = FALSE;
		/* set the status */
		fun_update_status (_("The frugalware update notifier daemon is not running. Update checking is disabled"));
		/* display an error */
		fun_error (_("Update checking disabled"), _(error_msg));
		/* start the connection retry timeout */
		g_timeout_add_seconds (45, (GSourceFunc)fun_timeout_conn, NULL);
		return;
	}
	seconds = fun_config_get_value_int ("update_interval") * 60;
	connected = TRUE;
	/* perform the first-run */
	/* we do this because when a database update is done for the first time 
	 * it takes a reasonable amount of time and the client may fail to get 
	 * any update status from the daemon */
	while (gtk_events_pending()) gtk_main_iteration ();
	fun_dbus_perform_service (PERFORM_UPDATE, NULL, &plist, NULL);

	/* register the timeout */
	g_timeout_add_seconds (seconds, (GSourceFunc)fun_timeout_func, NULL);
	/* set the status */
	fun_update_status (_("Idle"));

	return;
}

static gboolean
fun_timeout_conn (void)
{
	if (connected == TRUE)
		return FALSE;
	if (fun_dbus_perform_service (TEST_SERVICE, NULL, NULL, NULL) == FALSE)
	{
		connected = FALSE;
	}
	else
	{
		connected = TRUE;
		g_timeout_add_seconds (20, (GSourceFunc)fun_timeout_func, NULL);
		//fun_tooltip_set_text2 (tooltip, "", FALSE);
		/* set the status to Idle */
		fun_update_status (_("Idle"));
		return FALSE;
	}

	return TRUE;
}

static gboolean
fun_timeout_func (void)
{
	gchar *plist = NULL;

	/* Don't do anything if we're not connected to the daemon */
	if (!connected)
		return TRUE;

	/* set the status to let the user know that fun is checking for an update */
	fun_update_status (_("Checking for new updates..."));
	while(gtk_events_pending()) gtk_main_iteration ();
	/* clear the updates list store and give a small delay */
	gtk_list_store_clear (GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(fun_updates_tvw))));
	sleep (2);
	/* if updates are available, popup a notification to notify the user 
	 * and also populate the updates list in main window. BUT, before that
	 * disable the "check" button so that the user doesn't interrupt the 
	 * checking process */
	gtk_widget_set_sensitive (fun_check_btn, FALSE);
	if (fun_dbus_perform_service (PERFORM_UPDATE, NULL, &plist, NULL)==TRUE)
	{
		/* populate the update list */
		fun_populate_updates_tvw (plist);
		fun_tooltip_set_text (_("Updates are available"), _("New package updates are available for your system. Click the tray icon for more details"));
		fun_tooltip_show (icon);
	}

	/* re-enable the "check" button now */
	gtk_widget_set_sensitive (fun_check_btn, TRUE);
	/* and the status back to Idle */
	fun_update_status (_("Idle"));

	return TRUE;
}

static void
fun_about_show (void)
{
	if (fun_about_dlg==NULL)
	{
		gchar *ver = NULL;
		GList *list;

		if (!fun_about_pixbuf)
			fun_about_pixbuf = fun_get_icon ("fun", 128);
		ver = g_strdup_printf ("%s", VERSION);
		fun_about_dlg = gtk_about_dialog_new ();
		gtk_about_dialog_set_name (GTK_ABOUT_DIALOG(fun_about_dlg), PACKAGE);
		gtk_about_dialog_set_version (GTK_ABOUT_DIALOG(fun_about_dlg), ver);
		gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG(fun_about_dlg), _("(C) 2007 Frugalware Developer Team (GPL)"));
		gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG(fun_about_dlg), _("Frugalware Update Notifier"));
		gtk_about_dialog_set_license (GTK_ABOUT_DIALOG(fun_about_dlg), license);
		gtk_about_dialog_set_website (GTK_ABOUT_DIALOG(fun_about_dlg), "http://www.frugalware.org/");
		gtk_about_dialog_set_website_label (GTK_ABOUT_DIALOG(fun_about_dlg), "http://www.frugalware.org/");
		gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG(fun_about_dlg), fun_about_pixbuf);
		gtk_about_dialog_set_wrap_license (GTK_ABOUT_DIALOG(fun_about_dlg), TRUE);
		gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG(fun_about_dlg), authors);
		gtk_about_dialog_set_artists (GTK_ABOUT_DIALOG(fun_about_dlg), artists);
		gtk_about_dialog_set_translator_credits (GTK_ABOUT_DIALOG(fun_about_dlg), translators);
		g_signal_connect (G_OBJECT(fun_about_dlg), "destroy", G_CALLBACK(gtk_widget_destroyed), &fun_about_dlg);

		list = gtk_container_get_children (GTK_CONTAINER((GTK_DIALOG(fun_about_dlg))->action_area));
		list = list->next;
		list = list->next;
		g_signal_connect (G_OBJECT(list->data), "clicked", G_CALLBACK(fun_about_hide), NULL);
		g_free (ver);
	}

	gtk_widget_show (fun_about_dlg);

	return;
}

static void
fun_populate_updates_tvw (gchar *plist)
{
	char 			*pkg = NULL;
	GtkListStore	*store = NULL;
	GtkTreeIter		iter;
	GList			*l = NULL;
	GdkPixbuf		*icon = NULL;

	/* convert the updates string to a GList */
	GList	*pack_list = NULL;
	pkg = strtok (plist, " ");
	pack_list = g_list_append (pack_list, (gpointer)g_strdup(pkg));
	while ((pkg=strtok(NULL, " "))!=NULL) pack_list = g_list_append (pack_list, (gpointer)g_strdup(pkg));
	
	/* populate the updates treeview store */
	icon = fun_get_icon ("fun", 16);
	store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW(fun_updates_tvw)));
	for (l = g_list_first (pack_list); l; l = g_list_next (l))
	{
		gchar *ver = NULL;
		gchar *desc = NULL;
		if (fun_dbus_perform_service (GET_PACKAGE_INFO, l->data, &ver, &desc))
		{
			gtk_list_store_append (store, &iter);
			gtk_list_store_set (store, &iter, 0, icon, 1, l->data, 2, ver, 3, desc, -1);
		}
		else
		{
			fun_error (_("Error getting update information"), _("There was an error getting update information"));
			break;
		}
	}
	g_object_unref (icon);
	
	return;
}

static void
fun_about_hide (void)
{
	gtk_widget_hide (fun_about_dlg);

	return;
}

static void
fun_main_window_hide (void)
{
	gtk_widget_hide (fun_main_window);

	return;
}

static void
fun_main_window_show (void)
{
	/* Toggle window visibility */
	if (!GTK_WIDGET_VISIBLE(fun_main_window))
		gtk_widget_show (GTK_WIDGET(fun_main_window));
	else
		fun_main_window_hide ();

	return;
}

static GdkPixbuf *
fun_get_icon (const char *icon, int size)
{
	GtkIconTheme	*icon_theme = NULL;
	GdkPixbuf		*ret = NULL;
	GError			*error = NULL;
	
	icon_theme = gtk_icon_theme_get_default ();
	ret = gtk_icon_theme_load_icon (icon_theme,	icon, size, 0, &error);

	return ret;
}

static void
fun_update_status (const char *message)
{
	guint ci;
	
	if (!message)
		return;
	ci = gtk_statusbar_get_context_id (GTK_STATUSBAR(fun_statusbar), "-");
	gtk_statusbar_push (GTK_STATUSBAR(fun_statusbar), ci, message);
	
	return;
}

static void
fun_launch_gfpm (void)
{
	gchar *cmdline = NULL;
	gchar *su = fun_config_get_value_string ("gfpm_launcher");
	
	cmdline = g_strdup_printf ("%s gfpm", su);
	system (cmdline);
	g_free (cmdline);
	
	return;
}
