/*
 *  fun-messages.c for fun
 *
 *  This code is borrowed from gnetconfig and gfpm
 *  gnetconfig and gfpm is Copyright (C) 2006-2007 by Priyank Gosalia <priyankmg@gmail.com>
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

#define _GNU_SOURCE
#include "fun-messages.h"

void
fun_error (const char *message_title, const char *error_str)
{
	GtkWidget *error_dlg = NULL;

	if (!strlen(error_str))
		return;

	error_dlg = gtk_message_dialog_new (GTK_WINDOW(NULL),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_OK,
					"%s",
					error_str);
	gtk_window_set_resizable (GTK_WINDOW(error_dlg), FALSE);
	gtk_window_set_title (GTK_WINDOW(error_dlg), message_title);
	gtk_dialog_run (GTK_DIALOG(error_dlg));
	gtk_widget_destroy (error_dlg);

	return;
}

void
fun_message (const char *message_title, const char *message_str)
{
	GtkWidget *message_dlg;

	message_dlg = gtk_message_dialog_new (GTK_WINDOW(NULL),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_INFO,
					GTK_BUTTONS_OK,
					"%s",
					message_str);
	gtk_window_set_resizable (GTK_WINDOW(message_dlg), FALSE);
	gtk_window_set_title (GTK_WINDOW(message_dlg), message_title);
	gtk_dialog_run (GTK_DIALOG(message_dlg));
	gtk_widget_destroy (message_dlg);

	return;
}

gint
fun_question (const char *message_title, const char *message_str)
{
	GtkWidget 	*dialog;
	gint 		ret;

	dialog = gtk_message_dialog_new (GTK_WINDOW(NULL),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_YES_NO,
					"%s",
					message_str);
	gtk_window_set_resizable (GTK_WINDOW(dialog), FALSE);
	gtk_window_set_title (GTK_WINDOW(dialog), message_title);
	ret = gtk_dialog_run (GTK_DIALOG(dialog));
	gtk_widget_destroy (dialog);

	return ret;
}
/*
void
gfpm_plist_message (const char *message_title, const char *main_msg, GtkMessageType type, GList *packages)
{
	GtkWidget		*dialog;
	GtkListStore		*store;
	GtkScrolledWindow	*swindow;
	GtkCellRenderer		*r;
	GtkTreeIter		iter;
	GtkWidget		*tvw;
	GList			*l;

	if (packages == NULL)
		return;
	dialog = gtk_message_dialog_new (GTK_WINDOW(gfpm_mw),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					type,
					GTK_BUTTONS_CLOSE,
					"%s",
					main_msg);
	swindow = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new (NULL, NULL));
	gtk_scrolled_window_set_policy (swindow, GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (swindow, GTK_SHADOW_OUT);
	tvw = gtk_tree_view_new ();
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(tvw), FALSE);
	gtk_container_add (GTK_CONTAINER(swindow), tvw);
	store = gtk_list_store_new (1, G_TYPE_STRING);
	r = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tvw), -1, _("Package"), r, "text", 0, NULL);
	for (l=g_list_first(packages);l;l=g_list_next(l))
	{
		char *pkgstring = (char*)l->data;
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, pkgstring, -1);
		g_free (pkgstring);
	}
	gtk_tree_view_set_model (GTK_TREE_VIEW(tvw), GTK_TREE_MODEL(store));
	gtk_widget_set_size_request (tvw, 330, 120);
	gtk_widget_show (tvw);
	gtk_box_pack_start (GTK_BOX(GTK_DIALOG(dialog)->vbox), GTK_WIDGET(swindow), FALSE, FALSE, 0);
	gtk_widget_show_all (GTK_DIALOG(dialog)->vbox);
	gtk_window_set_resizable (GTK_WINDOW(dialog), TRUE);
	gtk_window_set_title (GTK_WINDOW(dialog), message_title);
	gtk_dialog_run (GTK_DIALOG(dialog));

	gtk_widget_destroy (dialog);

	return;
}
*/
