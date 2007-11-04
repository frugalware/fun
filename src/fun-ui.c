/*
 *  fun-ui.c for fun
 *
 *  Copyright (C) 2007 by Priyank Gosalia <priyankmg@gmail.com>
 *  Portions of this code are borrowed from gimmix
 *  gimmix is Copyright (C) 2006-2007 Priyank Gosalia <priyankmg@gmail.com>
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

#include <gtk/gtk.h>
#include "fun-config.h"
#include "fun-messages.h"
#include "fun-tooltip.h"
#include "fun-dbus.h"
#include "sexy-tooltip.h"
#include "eggtrayicon.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

static void fun_about_show (void);
static void fun_about_hide (void);

#define FUN_ICON  			"fun.png"
#define FUN_TOOLTIP_ICON 	"fun.png"

EggTrayIcon	*icon = NULL;
FunTooltip	*tooltip = NULL;
GtkWidget	*stooltip;

static GtkStatusIcon	*fun_icon = NULL;
static GtkWidget		*fun_about_dlg = NULL;
static GdkPixbuf		*fun_about_pixbuf = NULL;
static GtkWidget		*fun_config_dlg = NULL;
static GtkAdjustment 	*fun_config_upd_int_adj = NULL;
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

static const gchar translators[] = "";


static gboolean fun_timeout_func (void);
static gboolean fun_timeout_conn (void);

static void fun_config_dialog_show (void);
static void fun_restart (void);

static gboolean	cb_fun_systray_icon_clicked (GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean cb_fun_systray_enter_notify (GtkWidget *widget, GdkEventCrossing *event, gpointer data);
static gboolean cb_fun_systray_leave_notify (GtkWidget *widget, GdkEventCrossing *event, gpointer data);

static void cb_fun_config_dlg_close_clicked (GtkWidget *button, gpointer data);

void
fun_systray_create (void)
{
	gchar 		*icon_file;
	GdkPixbuf	*icon_image;
	GdkPixbuf	*icon_tooltip;
	GtkWidget	*systray_icon;
	GdkColor 	color;
	
	icon_file = g_strdup ("/usr/share/fun/fun.png");
	/* create the tray icon */
	icon = egg_tray_icon_new ("Frugalware Update Notifier");
	icon_image = gdk_pixbuf_new_from_file_at_size (icon_file, 20, 20, NULL);
	systray_icon = gtk_image_new_from_pixbuf (icon_image);
	g_free (icon_file);
	gtk_container_add (GTK_CONTAINER (icon), systray_icon);
	g_object_unref (icon_image);
	
	stooltip = sexy_tooltip_new ();
	gdk_color_parse ("white", &color);
	gtk_widget_modify_bg (GTK_WIDGET(stooltip), GTK_STATE_NORMAL, &color);
	
	/* set the default tooltip */
	tooltip = fun_tooltip_new ();
	fun_tooltip_set_text1 (tooltip, "Frugalware Update Notifier", TRUE);
	icon_file = g_strdup ("/usr/share/fun/fun.png");
	icon_tooltip = gdk_pixbuf_new_from_file_at_size (icon_file, 32, 32, NULL);
	g_free (icon_file);
	fun_tooltip_set_icon (tooltip, icon_tooltip);
	g_object_unref (icon_tooltip);
	
	g_signal_connect (icon, "button-press-event", G_CALLBACK (cb_fun_systray_icon_clicked), NULL);
	g_signal_connect (icon, "enter-notify-event", G_CALLBACK(cb_fun_systray_enter_notify), NULL);
	g_signal_connect (icon, "leave-notify-event", G_CALLBACK(cb_fun_systray_leave_notify), NULL);
	gtk_widget_show (GTK_WIDGET(systray_icon));
	gtk_widget_show (GTK_WIDGET(icon));
	
	gtk_widget_ref (tooltip->hbox);
	gtk_container_remove (GTK_CONTAINER(tooltip->window), tooltip->hbox);
	gtk_container_add (GTK_CONTAINER(stooltip), tooltip->hbox);
	gtk_widget_unref (tooltip->hbox);
	
	return;
}

static gboolean
cb_fun_systray_enter_notify (GtkWidget *widget, GdkEventCrossing *event, gpointer data)
{
	GdkScreen *screen = NULL;
	GdkScreen *def_screen = NULL;
	GdkRectangle rectangle;
	gint x, y;
	gint w, h;
	gint top;
	
	/* Check where to place our tooltip */
	def_screen = gdk_screen_get_default ();
	w = gdk_screen_get_width (def_screen);
	h = gdk_screen_get_height (def_screen);
	/* Get the location of the system tray icon */
	gdk_window_get_origin ((GTK_WIDGET(icon)->window), &x, &y);
	if (h-y >= 100)
		top = 1; /* tooltip should be placed on top */
	else
		top = 0; /* tooltip should be placed on bottom */
	w = h = 0;
	
	/* Move the tooltip off-screen to calculate the exact co-ordinates */
	rectangle.x = 2500;
	rectangle.y = 2500;
	rectangle.width = 100;
	rectangle.height = 70;
	screen = gtk_widget_get_screen (GTK_WIDGET(icon));
	sexy_tooltip_position_to_rect (SEXY_TOOLTIP(stooltip), &rectangle, screen);
	gtk_widget_show_all (stooltip);
	gtk_window_get_size (GTK_WINDOW(stooltip), &w, &h);
	
	/* Good, now lets move it back to where it should be */
	if (top == 1)
	{	
		rectangle.x = x-(w/4);
		rectangle.y = y-25;
	}
	else
	{
		rectangle.x = x-(w/4);
		rectangle.y = y-130;
	}

	sexy_tooltip_position_to_rect (SEXY_TOOLTIP(stooltip), &rectangle, screen);

	return TRUE;
}

static gboolean
cb_fun_systray_leave_notify (GtkWidget *widget, GdkEventCrossing *event, gpointer data)
{
	fun_tooltip_hide (tooltip);
	gtk_widget_hide (stooltip);
	
	return TRUE;
}

static gboolean
cb_fun_systray_icon_clicked (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->button == 3)
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
		if (fun_question("Restart Fun", "Fun needs to be restarted in order for the changes to take effect. Do you want to restart Fun now ?") == GTK_RESPONSE_YES)
		{
			fun_restart ();
		}
	}
	gtk_widget_hide (fun_config_dlg);
	
	return;
}

void
fun_ui_cleanup (void)
{
	if (icon == NULL)
		return;
	gtk_widget_destroy (GTK_WIDGET(icon));
	gtk_widget_destroy (GTK_WIDGET(fun_config_dlg));
	fun_tooltip_destroy (tooltip);
	icon = NULL;
	tooltip = NULL;
	
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
	GtkWidget *vbox1;
	GtkWidget *hbox2;
	GtkWidget *image1;
	GtkWidget *label2;
	GtkWidget *hbox1;
	GtkWidget *label1;
	GtkObject *spinbutton1_adj;
	GtkWidget *spinbutton1;
	GtkWidget *hbuttonbox1;
	GtkWidget *pref_close;
	
	fun_config_dlg = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW(fun_config_dlg), 350, 150);
	//gtk_window_set_resizable (fun_config_dlg, FALSE);
	
	vbox1 = gtk_vbox_new (FALSE, 5);
	gtk_widget_show (vbox1);
	gtk_container_add (GTK_CONTAINER (fun_config_dlg), vbox1);
	gtk_container_set_border_width (GTK_CONTAINER (vbox1), 6);

	hbox2 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox2);
	gtk_box_pack_start (GTK_BOX (vbox1), hbox2, FALSE, TRUE, 5);

	image1 = gtk_image_new_from_file ("/usr/share/fun/fun.png");
	gtk_widget_show (image1);
	gtk_box_pack_start (GTK_BOX (hbox2), image1, FALSE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (image1), 0, 0);

	label2 = gtk_label_new ("<b>Configure Frugalware Update Nofitier</b>");
	gtk_widget_show (label2);
	gtk_box_pack_start (GTK_BOX (hbox2), label2, TRUE, TRUE, 0);
	gtk_label_set_use_markup (GTK_LABEL (label2), TRUE);
	gtk_misc_set_alignment (GTK_MISC (label2), 0.05, 0.5);

	hbox1 = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox1);
	gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

	label1 = gtk_label_new ("<b>Update Interval (minutes) :</b>");
	gtk_widget_show (label1);
	gtk_box_pack_start (GTK_BOX (hbox1), label1, TRUE, TRUE, 0);
	gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);
	gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

	fun_config_upd_int_adj = gtk_adjustment_new (1, 1, 60, 1, 10, 10);
	spinbutton1 = gtk_spin_button_new (GTK_ADJUSTMENT (fun_config_upd_int_adj), 1, 0);
	gtk_widget_show (spinbutton1);
	gtk_box_pack_start (GTK_BOX (hbox1), spinbutton1, FALSE, TRUE, 0);

	hbuttonbox1 = gtk_hbutton_box_new ();
	gtk_widget_show (hbuttonbox1);
	gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_END);

	pref_close = gtk_button_new_from_stock ("gtk-close");
	gtk_widget_show (pref_close);
	gtk_container_add (GTK_CONTAINER (hbuttonbox1), pref_close);
	GTK_WIDGET_SET_FLAGS (pref_close, GTK_CAN_DEFAULT);
	g_signal_connect (G_OBJECT(pref_close), "clicked", G_CALLBACK(cb_fun_config_dlg_close_clicked), (gpointer)spinbutton1);
	
	return;
}

void
fun_ui_init (void)
{
	GError		*error = NULL;
	gulong		seconds = 0;
	
	fun_systray_create ();
	fun_config_dialog_init ();
	
	if (fun_dbus_perform_service (TEST_SERVICE, NULL) == FALSE)
	{
		g_print ("Failed to connect to the fun daemon\n");
		connected = FALSE;
		/* start the connection retry timeout */
		g_timeout_add_seconds (30, (GSourceFunc)fun_timeout_conn, NULL);
		return;
	}
	seconds = fun_config_get_value_int ("update_interval") * 60;
	connected = TRUE;

	/* register the timeout */
	g_timeout_add_seconds (seconds, (GSourceFunc)fun_timeout_func, NULL);

	return;
}

static gboolean
fun_timeout_conn (void)
{
	if (connected == TRUE)
		return FALSE;
	if (fun_dbus_perform_service (TEST_SERVICE, NULL) == FALSE)
	{
		connected = FALSE;
	}
	else
	{
		connected = TRUE;
		g_timeout_add_seconds (20, (GSourceFunc)fun_timeout_func, NULL);
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

	if (fun_dbus_perform_service (PERFORM_UPDATE, &plist)==TRUE)
	{
		//g_print ("\nlist is\n %s", plist);
		//g_print ("Yeaahaaw! success\n");
		fun_tooltip_set_text1 (tooltip, "Updates are available", TRUE);
		fun_tooltip_set_text2 (tooltip, "Click here to know more..", TRUE);
		fun_tooltip_show (tooltip);
		cb_fun_systray_enter_notify (NULL, NULL, NULL);
	}
	else
		g_print ("Damn !\n");

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
			fun_about_pixbuf = gdk_pixbuf_new_from_file ("/usr/share/fun/fun.png", NULL);
		ver = g_strdup_printf ("%s", PACKAGE_VERSION);
		fun_about_dlg = gtk_about_dialog_new ();
		gtk_about_dialog_set_name (GTK_ABOUT_DIALOG(fun_about_dlg), PACKAGE);
		gtk_about_dialog_set_version (GTK_ABOUT_DIALOG(fun_about_dlg), ver);
		gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG(fun_about_dlg), "(C) 2007 Frugalware Developer Team (GPL)");
		gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG(fun_about_dlg), "Frugalware Update Notifier");
		gtk_about_dialog_set_license (GTK_ABOUT_DIALOG(fun_about_dlg), NULL);
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
fun_about_hide (void)
{
	gtk_widget_hide (fun_about_dlg);

	return;
}


