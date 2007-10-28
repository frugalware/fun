#ifndef _FUN_TOOLTIP_H
#define _FUN_TOOLTIP_H

#include <gtk/gtk.h>

/* FunTooltip structure */
typedef struct {
	GtkWidget *window;
	GtkWidget *hbox;
	GtkWidget *vbox;
	GtkWidget *icon;
} FunTooltip;

/* Fun Tooltip Functions */

/* Create a new tooltip */
FunTooltip *fun_tooltip_new (void);

/* Sets the tooltip text (label1) */
void fun_tooltip_set_text1 (FunTooltip *tooltip, const gchar *text, gboolean formatting);

/* Sets the tooltip text (label2) */
void fun_tooltip_set_text2 (FunTooltip *tooltip, const gchar *text, gboolean formatting);

/* Sets the icon for the tooltip */
void fun_tooltip_set_icon (FunTooltip *tooltip, GdkPixbuf *pixbuf);

/* Show the tooltip */
void fun_tooltip_show (FunTooltip *tooltip);

/* Hide the tooltip */
void fun_tooltip_hide (FunTooltip *tooltip);

/* Destroy the tooltip object and free the memory */
void fun_tooltip_destroy (FunTooltip *tooltip);

#endif

