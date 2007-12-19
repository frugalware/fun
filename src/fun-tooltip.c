/*
 *  fun-tooltip.c for fun
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

#include <gtk/gtk.h>
#include <libnotify/notification.h>
#include <libnotify/notify.h>
#include "fun-config.h"
#include "fun-tooltip.h"

static NotifyNotification *tooltip = NULL;

void fun_tooltip_new (GtkStatusIcon *icon)
{
	tooltip = notify_notification_new ("Frugalware Update Notifier",
										NULL,
										"fun",
										NULL);
	notify_notification_set_category (tooltip, "information");
	notify_notification_set_timeout (tooltip, (fun_config_get_value_int("notification_timeout")*1000));
	notify_notification_set_urgency (tooltip, NOTIFY_URGENCY_NORMAL);
	notify_notification_attach_to_status_icon (tooltip, icon);
	
	return;
}

void fun_tooltip_set_text (const gchar *summary, const gchar *body)
{
	if (tooltip)
		notify_notification_update (tooltip, summary, body, "fun");

	return;
}

void fun_tooltip_set_notification_timeout (guint timeout)
{
	notify_notification_set_timeout (tooltip, timeout*1000);
	
	return;
}

void fun_tooltip_show (GtkStatusIcon *icon)
{
	if (tooltip)
	{
		GdkScreen *screen = NULL;
		GdkRectangle area;
		
		gtk_status_icon_get_geometry (icon, &screen, &area, NULL);
		notify_notification_set_geometry_hints (tooltip, screen, area.x, area.y);
		notify_notification_show (tooltip, NULL);
	}

	return;
}

void fun_tooltip_destroy (void)
{
	g_object_unref (tooltip);
	notify_uninit ();
	tooltip = NULL;

	return;
}
