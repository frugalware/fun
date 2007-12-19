#ifndef _FUN_TOOLTIP_H
#define _FUN_TOOLTIP_H

#include <gtk/gtk.h>
#include <libnotify/notification.h>
#include <libnotify/notify.h>

/* Fun Tooltip Functions */

/* Create a new tooltip */
NotifyNotification *fun_tooltip_new (GtkStatusIcon *icon);

/* Sets the tooltip text (label1) */
void fun_tooltip_set_text (NotifyNotification *tooltip, const gchar *summary, const gchar *body);

/* Show the tooltip */
void fun_tooltip_show (GtkStatusIcon *icon, NotifyNotification *tooltip);

/* Destroy the tooltip object and free the memory */
void fun_tooltip_destroy (NotifyNotification *tooltip);

#endif

