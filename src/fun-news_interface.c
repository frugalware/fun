/*
 *  fun-news_interface.c for fun
 *
 *  Copyright (C) 2008 by Priyank Gosalia <priyankmg@gmail.com>
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
#include "fun-ui.h"
#include "fun-config.h"
#include "fun-messages.h"
#include "fun-tooltip.h"
#include "fun-news_backend.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern GladeXML		*xml;
extern GtkStatusIcon	*icon;

static GtkWidget	*fun_news_tvw = NULL;
static GtkWidget	*fun_news_txtvw = NULL;
static GtkWidget	*fun_news_visitlink_btn = NULL;

static void fun_news_interface_populate_newslist (void);
static void fun_news_interface_display_news_for_id (guint id);
static gboolean fun_news_prefetch_func (void);
static gboolean fun_news_check_func (void);

/* CALLBACKS */
static void cb_fun_news_tvw_selected (GtkTreeSelection *selection, gpointer data);
static void cb_fun_news_visit_link_clicked (GtkButton *button, gpointer data);

void
fun_news_interface_init (void)
{
	GtkListStore		*store = NULL;
	GtkCellRenderer		*renderer = NULL;
	GtkTreeViewColumn	*column = NULL;
	GtkTreeSelection	*selection = NULL;
	guint			seconds;

	fun_news_tvw = glade_xml_get_widget (xml, "fun_news_treeview");
	fun_news_txtvw = glade_xml_get_widget (xml, "fun_news_textview");
	fun_news_visitlink_btn = glade_xml_get_widget (xml, "fun_visitlink_btn");

	store = gtk_list_store_new (3,
				G_TYPE_UINT,		/* News ID */
				G_TYPE_STRING,		/* News Title */
				G_TYPE_STRING);  	/* News Date */

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("ID"),
							renderer,
							"text", 0,
							NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_column_set_expand (column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(fun_news_tvw), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Title"),
							renderer,
							"text", 1,
							NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_column_set_expand (column, TRUE);
	gtk_tree_view_column_set_min_width (column, 80);
	gtk_tree_view_append_column (GTK_TREE_VIEW(fun_news_tvw), column);
	
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Date"),
							renderer,
							"text", 2,
							NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);
	gtk_tree_view_column_set_expand (column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(fun_news_tvw), column);

	gtk_tree_view_set_model (GTK_TREE_VIEW(fun_news_tvw), GTK_TREE_MODEL(store));
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(fun_news_tvw));
	g_signal_connect (selection, "changed", G_CALLBACK(cb_fun_news_tvw_selected), NULL);
	g_signal_connect (fun_news_visitlink_btn, "clicked", G_CALLBACK(cb_fun_news_visit_link_clicked), NULL);
	
	fun_news_interface_populate_newslist ();
	/* fetch latest news xml 1 minute earlier than the check */
	seconds = (fun_config_get_value_int ("news_interval") * 60) - 60;
	g_timeout_add_seconds (seconds, (GSourceFunc)fun_news_prefetch_func, NULL);
	/* the news check timeout */
	seconds += 60;
	g_timeout_add_seconds (seconds, (GSourceFunc)fun_news_check_func, NULL);

	return;
}

static void
fun_news_interface_populate_newslist (void)
{
	GList		*list = NULL;
	GtkListStore	*store = NULL;
	GtkTreeIter	iter;
	
	list = fun_get_existing_news_list ();
	store = GTK_LIST_STORE (gtk_tree_view_get_model(GTK_TREE_VIEW(fun_news_tvw)));
	while (list != NULL)
	{
		NewsItem	*item = NULL;
		char		title[255] = "";
		char		date[255] = "";
		
		item = (NewsItem*) list->data;
		strcpy (title, item->title);
		g_strstrip (title);
		strcpy (date, item->date);
		g_strstrip (date);
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, item->id, 1, g_strdelimit(title,"\n",0), 2, date, -1);
		list = g_list_next (list);
	}

	return;
}

static void
fun_news_interface_display_news_for_id (guint id)
{
	GList		*list = NULL;
	GtkTextBuffer	*buffer = NULL;
	GtkTextIter	iter;
	gboolean	flag = FALSE;

	list = fun_get_existing_news_list ();
	while (list != NULL)
	{
		NewsItem	*item = NULL;
		item = (NewsItem*) list->data;
		if (item->id == id)
		{
			buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(fun_news_txtvw));
			gtk_text_buffer_set_text (buffer, "", 0);
			gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
			gtk_text_buffer_insert (buffer, &iter, "\n", -1);
			gtk_text_buffer_insert (buffer, &iter, item->title, -1);
			gtk_text_buffer_insert (buffer, &iter, "\n\n", -1);
			gtk_text_buffer_insert (buffer, &iter, item->description, -1);
			flag = TRUE;
			break;
		}
		list = g_list_next (list);
	}
	if (flag) gtk_text_view_set_buffer (GTK_TEXT_VIEW(fun_news_txtvw), buffer);

	return;
}

static gboolean
fun_news_prefetch_func (void)
{
	fun_fetch_news_xml ();

	return TRUE;
}

static gboolean
fun_news_check_func (void)
{
	GList		*latest = NULL;
	NewsItem	*lastitem = NULL;

	/* see if updates are available */
	latest = fun_compare_lists (fun_get_existing_news_list(), fun_get_new_news_list());
	while (latest != NULL)
	{
		NewsItem *i = NULL;
		
		i = latest->data;
		fun_save_news_to_file (i);
		lastitem = i;
		latest = g_list_next (latest);
	}
	if (lastitem)
	{
		/* re-populate the existing list */
		fun_populate_existing_news_list ();
		/* display a notification */
		fun_tooltip_set_text (_("Latest News"), lastitem->title);
		fun_tooltip_show (icon);
		fun_news_interface_populate_newslist ();
	}
	return TRUE;
}

static void
cb_fun_news_tvw_selected (GtkTreeSelection *selection, gpointer data)
{
	GtkTreeModel	*model;
	GtkTreeIter	iter;
	guint		id = 0;

	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, 0, &id, -1);
		fun_news_interface_display_news_for_id (id);
	}

	return;
}

static void
cb_fun_news_visit_link_clicked (GtkButton *button, gpointer data)
{
	GtkTreeSelection	*selection;
	GtkTreeModel		*model;
	GtkTreeIter		iter;
	guint			id = 0;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(fun_news_tvw));
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		char *command = NULL;
		gtk_tree_model_get (model, &iter, 0, &id, -1);
		command = g_strdup_printf ("%s %s",
					fun_config_get_browser_path(fun_config_get_value_string("news_browser")),
					fun_news_get_url_for_id (id));
		g_print ("%s\n", command);
		system (command);
		g_free (command);
	}

	return;
}


