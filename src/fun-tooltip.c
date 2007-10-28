/*
 *  fun-tooltip.c for fun
 *
 *  Copyright (C) 2007 by Priyank Gosalia <priyankmg@gmail.com>
 *  Portions of this code are borrowed fron gimmix
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
#include <stdio.h>
#include "fun-tooltip.h"

FunTooltip *fun_tooltip_new (void)
{
	FunTooltip	*tooltip;
	GtkWidget	*label1;
	GtkWidget	*label2;
	
	/* main tooltip window */
	tooltip = g_malloc (sizeof(FunTooltip));
	tooltip->window = gtk_window_new (GTK_WINDOW_POPUP);
	
	gtk_window_set_resizable (GTK_WINDOW(tooltip->window), FALSE);
	gtk_window_set_decorated (GTK_WINDOW(tooltip->window), FALSE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW(tooltip->window), TRUE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW(tooltip->window), TRUE);

	/* the two main layout boxes */
	tooltip->hbox = gtk_hbox_new (FALSE, 4);
	tooltip->vbox = gtk_vbox_new (FALSE, 0);
	
	/* pack the boxes */
	gtk_container_add (GTK_CONTAINER(tooltip->window), tooltip->hbox);
	gtk_box_pack_end (GTK_BOX(tooltip->hbox), tooltip->vbox, FALSE, FALSE, 2);
	
	/* tooltip icon */
	tooltip->icon = gtk_image_new_from_pixbuf (NULL);
	gtk_misc_set_padding (GTK_MISC(tooltip->icon), 4, 4);
	gtk_box_pack_start (GTK_BOX(tooltip->hbox), tooltip->icon, TRUE, TRUE, 0);

	/* labels */
	label1 = gtk_label_new (NULL);
	g_object_set (G_OBJECT(label1), "use-markup", TRUE, NULL);
	gtk_box_pack_start (GTK_BOX(tooltip->vbox), label1, TRUE, FALSE, 1);
	gtk_misc_set_alignment (GTK_MISC(label1), 0, 0);
	label2 = gtk_label_new (NULL);
	g_object_set (G_OBJECT(label2), "use-markup", TRUE, NULL);
	gtk_box_pack_start (GTK_BOX(tooltip->vbox), label2, TRUE, FALSE, 1);
	gtk_misc_set_alignment (GTK_MISC(label2), 0, 0);

	return tooltip;
}

void fun_tooltip_set_text1 (FunTooltip *tooltip, const gchar *text, gboolean formatting)
{
	GList *list;
	gchar *markup;

	if ( (list = gtk_container_get_children (GTK_CONTAINER(tooltip->vbox))) != NULL )
	{
		if (text == NULL)
		{
			gtk_label_set_text (GTK_LABEL(list->data), NULL);
			gtk_widget_hide (GTK_WIDGET(list->data));
			return;
		}
		
		if (formatting == TRUE)
		{
			markup = g_markup_printf_escaped ("<span size=\"large\" weight=\"bold\">%s</span>", text);
			gtk_label_set_markup (GTK_LABEL(list->data), markup);
			g_free (markup);
		}
		else
		{
			gtk_label_set_text (GTK_LABEL(list->data), text);
		}

		g_list_free (list);
	}

	return;
}

void fun_tooltip_set_text2 (FunTooltip *tooltip, const gchar *text, gboolean formatting)
{
	GList *list;
	gchar *markup;

	if ( (list = gtk_container_get_children (GTK_CONTAINER(tooltip->vbox))) != NULL )
	{
		if ((list = g_list_nth (list, 1)) == NULL)
			return;

		if (text == NULL)
		{
			gtk_label_set_text (GTK_LABEL(list->data), NULL);
			gtk_widget_hide (GTK_WIDGET(list->data));
			return;
		}
		if (formatting == TRUE)
		{
			markup = g_markup_printf_escaped ("<span size=\"medium\"><i>%s</i></span>", text);
			gtk_label_set_markup (GTK_LABEL(list->data), markup);
			g_free (markup);
		}
		else
		{
			gtk_label_set_text (GTK_LABEL(list->data), text);
		}
		gtk_widget_show (GTK_WIDGET(list->data));
		g_list_free (list);
	}

	return;
}

void fun_tooltip_set_icon (FunTooltip *tooltip, GdkPixbuf *pixbuf)
{
	gtk_image_set_from_pixbuf (GTK_IMAGE(tooltip->icon), pixbuf);

	return;
}

void fun_tooltip_show (FunTooltip *tooltip)
{
	if (tooltip != NULL)
	{
		gtk_widget_show (GTK_WIDGET(tooltip->hbox));
		gtk_widget_show (GTK_WIDGET(tooltip->window));
	}
	return;
}

void fun_tooltip_hide (FunTooltip *tooltip)
{
	if (tooltip != NULL)
		gtk_widget_hide (GTK_WIDGET(tooltip->window));
	
	return;
}

void fun_tooltip_destroy (FunTooltip *tooltip)
{
	gtk_widget_destroy (GTK_WIDGET(tooltip->vbox));
	gtk_widget_destroy (GTK_WIDGET(tooltip->hbox));
	gtk_widget_destroy (GTK_WIDGET(tooltip->window));
	g_free (tooltip);

	return;
}

