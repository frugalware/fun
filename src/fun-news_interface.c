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

static GtkWidget	*fun_news_tvw = NULL;
static GtkWidget	*fun_news_txtvw = NULL;

static void fun_news_interface_populate_newslist (void);

void
fun_news_interface_init (void)
{
	GtkListStore		*store = NULL;
	GtkCellRenderer		*renderer = NULL;
	GtkTreeViewColumn	*column = NULL;

	fun_news_tvw = glade_xml_get_widget (xml, "fun_news_treeview");
	fun_news_txtvw = glade_xml_get_widget (xml, "fun_news_textview");

	store = gtk_list_store_new (2,
				G_TYPE_UINT,		/* News ID */
				G_TYPE_STRING);  	/* News Title */

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
	gtk_tree_view_set_model (GTK_TREE_VIEW(fun_news_tvw), GTK_TREE_MODEL(store));
	
	fun_news_interface_populate_newslist ();
	
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
		
		item = (NewsItem*) list->data;
		strcpy (title, item->title);
		g_strstrip (title);
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, item->id, 1, g_strdelimit(title,"\n",0), -1);
		list = g_list_next (list);
	}

	return;
}



