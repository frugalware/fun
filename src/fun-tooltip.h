#ifndef _FUN_TOOLTIP_H
#define _FUN_TOOLTIP_H

#include <gtk/gtk.h>

/* Fun Tooltip Functions */

/* Create a new tooltip */
void fun_tooltip_new (GtkStatusIcon *icon);

/* Sets the tooltip text */
void fun_tooltip_set_text (const gchar *summary, const gchar *body);

/* Sets the tooltip timeout (seconds) */
void fun_tooltip_set_notification_timeout (guint timeout);

/* Show the	tooltip */
void fun_tooltip_show (GtkStatusIcon *icon);

/* Destroy the tooltip object and free the memory */
void fun_tooltip_destroy (void);

#endif

