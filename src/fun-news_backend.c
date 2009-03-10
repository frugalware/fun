/*
 *  fun-news_backend.c for fun
 *
 *  Copyright (C) 2009 by Priyank Gosalia <priyankmg@gmail.com>
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

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <nxml.h>
#include "fun-news_backend.h"
#include "wejpconfig.h"

#define NEWS_URL	"http://frugalware.org/rss/news"
#define NEWS_XML	".fun/news/news.xml"
#define NEWS_ITEM_DIR	".fun/news"
#define NEWS_ITEM_LIST	".fun/news/list"

/* populated on every update */
static GList	*news_item_list = NULL;

/* populated on startup and updated regularly */
/* existing news item list */
static GList	*e_news_item_list = NULL;

static gboolean fetched = FALSE;

/**
 *
 * fun_newslist_compare_func:
 *
 * Compares two lists
 */
static gint
fun_newslist_compare_func (gconstpointer a, gconstpointer b)
{
	NewsItem	*item1 = NULL;
	NewsItem	*item2 = NULL;
	
	item1 = (NewsItem*) a;
	item2 = (NewsItem*) b;
	if (item1->id == item2->id)
	{
		return 0;
	}

	return 1;
}

/**
 *
 * fun_compare_lists:
 *
 * Compares two lists and returns items in newlist
 * which are not in the old list.
 * returns NULL if the two lists are equal
 */
GList*
fun_compare_lists (GList *oldlist, GList *newlist)
{
	GList *ret = NULL;
	GList *temp = NULL;
	
	temp = newlist;
	while (temp!=NULL)
	{
		GList *found = NULL;
		NewsItem *ni = NULL;
		ni = temp->data;
		
		found = g_list_find_custom (oldlist, ni, fun_newslist_compare_func);
		if (found == NULL)
		{
			ret = g_list_append (ret, ni);
		}
		
		temp = g_list_next (temp);
	}

	return ret;	
}

/**
 *
 * fun_glist_to_string:
 *
 * Converts a GList* to char*
 */
static char *
fun_glist_to_string (GList *list)
{
	GString	*new = NULL;
	char	*ret = NULL;
	GList	*temp = NULL;
	guint	lines = 0;
	
	new = g_string_new ("");
	temp = list;
	lines = g_list_length (temp);
	while (temp != NULL)
	{
		new = g_string_append (new, temp->data);
		temp = g_list_next (temp);
	}
	ret = g_strdup (new->str);
	g_string_free (new, TRUE);
	
	return ret;
}

/**
 *
 * fun_get_news_item_from_file:
 *
 * Reads from file and returns NewsItem* containing title and desc
 */
static NewsItem*
fun_get_news_item_from_file (const char *file)
{
	FILE *fp = NULL;
	NewsItem *ni = NULL;
	char line[PATH_MAX+1] = "";
	GList *dsclist = NULL;

	if (!(fp=fopen(file,"r")))
		return NULL;
	ni = (NewsItem*) malloc (sizeof(NewsItem));
	memset (ni, 0, sizeof(NewsItem));
	/* get the title */
	if (fgets(line,PATH_MAX,fp))
		sprintf (ni->title, line);
	/* get the publish date */
	if (fgets(line,PATH_MAX,fp))
		sprintf (ni->date, line);
	
	while (fgets(line,PATH_MAX,fp))
	{
		dsclist = g_list_append (dsclist, g_strdup(line));
		memset (line, 0, sizeof(line));
	}
	ni->description = fun_glist_to_string (dsclist);
	g_list_free (dsclist);
	fclose (fp);
	
	return ni;
}

/**
 *
 * fun_populate_existing_news_list:
 *
 * Populates a GList of NewsItem* from existing news on disk
 */
void
fun_populate_existing_news_list (void)
{
	FILE *fp = NULL;
	char line[PATH_MAX+1] = "";
	NewsItem *item = NULL;
	char *path = NULL;
	char *npath = NULL;
	
	path = cfg_get_path_to_config_file (NEWS_ITEM_LIST);
	if (!(fp=fopen(path,"r")))
	{
		//printf ("fun_populate_existing_news_list: couldn't open news item list \n");
		return;
	}
	g_free (path);
	while (fgets(line,PATH_MAX,fp))
	{
		g_strstrip (line);
		if (strlen(line))
		{
			path = g_strdup_printf ("%s/%s", NEWS_ITEM_DIR, line);
			npath = cfg_get_path_to_config_file (path);
			item = fun_get_news_item_from_file (npath);
			item->id = atoi (line);
			g_free (path);
			g_free (npath);
			if (item != NULL)
			e_news_item_list = g_list_append (e_news_item_list, (gpointer)item);
		}
	}
	fclose (fp);
	e_news_item_list = g_list_reverse (e_news_item_list);

	return;
}

/**
 *
 * fun_add_entry_to_newslist:
 * @id: id to be added to list
 *
 * Adds id to the news item list
 */
static int
fun_add_entry_to_newslist (gint id)
{
	FILE *fp = NULL;
	char *path = NULL;
	char line[PATH_MAX+1];
	int temp;
	
	path = cfg_get_path_to_config_file (NEWS_ITEM_LIST);
	fp = fopen (path, "r");
	if (fp != NULL)
	{
		/* check if the id is already present */
		while (fgets(line,PATH_MAX,fp))
		{
			/* this needs to be changed after news items exceed 99 */
			line[2] = 0;
			temp = atoi (line);
			//	printf ("%d checking \n", temp);
			if (temp == id)
			{
				// printf ("%d is already present\n", id);
				return -1;
			}
		}
		fclose (fp);
	}
	/* good, now add it */
	if (!(fp=fopen(path,"a")))
		return -1;
	fprintf (fp, "%d\n", id);
	fclose (fp);
	g_free (path);
	
	return 0;
}

/**
 *
 * fun_save_news_to_file:
 * @item: a pointer to the news item
 *
 * Saves a news item to file
 */
int
fun_save_news_to_file (NewsItem *item)
{
	FILE *fp = NULL;
	char *npath = NULL;
	char *path = NULL;

	if (item == NULL)
		return -1;
	npath = g_strdup_printf ("%s/%d", NEWS_ITEM_DIR, item->id);
	path = cfg_get_path_to_config_file (npath);
	
	if (fun_add_entry_to_newslist(item->id)!=0)
	{
		//g_print ("couldn't add to new list\n");
		return -1;
	}
	fp = fopen (path, "w");
	if (fp == NULL)
	{
		//g_print ("Error opening news file\n");
		return -1;
	}
	//printf ("saving news with id: %d\n", item->id);
	fprintf (fp, "%s\n", item->title);
	fprintf (fp, "%s\n", item->date);
	fprintf (fp, "%s\n", item->description);
	fclose (fp);
	g_free (path);
	g_free (npath);

	return 0;
}

/**
 *
 * fun_get_news_id_from_url:
 * @url: the url
 *
 * Extract news id from a url
 */
static int
fun_get_news_id_from_url (const char *url)
{
	char *tp = g_strdup (url);
	char *bk = tp;
	char *ptr = NULL;
	char id[4] = "";
	tp = g_strreverse (tp);
	ptr = id;
	while (*tp != NULL && *tp != '/')
		*ptr++ = *tp++;
	*ptr++ = 0;
	g_free (bk);
	ptr = id;
	ptr = g_strreverse (ptr);

	return (atoi(ptr));
}

static size_t
fun_news_write_func (void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fwrite (ptr, size, nmemb, stream);
}

static size_t
fun_news_read_func (void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fread (ptr, size, nmemb, stream);
}

void
fun_fetch_news_xml (void)
{
	gchar *path = NULL;
	nxml_t *nxml = NULL;
	nxml_data_t *nroot = NULL;
	nxml_data_t *ndata = NULL;
	nxml_data_t *nndata = NULL;
	char *str = NULL;
	nxml_error_t e;

	/* free the old news list */
	g_list_free (news_item_list);

	e = nxml_new (&nxml);
	nxml_parse_url (nxml, NEWS_URL);
	nxml_root_element (nxml, &nroot);
	nxml_find_element (nxml, nroot, "channel", &ndata);
	nxml_find_element (nxml, ndata, "item", &nndata);
	while (nndata)
	{
		nxml_data_t *child = NULL;
		nxml_data_t *d = NULL;
		child = nndata;

		NewsItem *newsitem = (NewsItem*) malloc (sizeof(NewsItem));
		memset (newsitem, 0, sizeof(NewsItem));
		
		/* title */
		nxml_find_element (nxml, child, "title", &d);
		nxml_get_string (d, &str);
		sprintf (newsitem->title, str);
		free (str);
		
		/* link */
		nxml_find_element (nxml, child, "link", &d);
		nxml_get_string (d, &str);
		newsitem->id = fun_get_news_id_from_url (str);
		free (str);
		
		/* description */
		nxml_find_element (nxml, child, "description", &d);
		nxml_get_string (d, &str);
		newsitem->description = g_strdup (str);
		free (str);
		
		/* pubdate */
		nxml_find_element (nxml, child, "pubDate", &d);
		nxml_get_string (d, &str);
		strncpy (newsitem->date, str, strlen(str));
		free (str);

		news_item_list = g_list_append (news_item_list, (gpointer)newsitem);
		nndata = nndata->next;
		
	}
	nxml_free (nxml);
	
	news_item_list = g_list_reverse (news_item_list);
	path = cfg_get_path_to_config_file (NEWS_ITEM_LIST);
	if (g_file_test(path,G_FILE_TEST_EXISTS)==FALSE)
	{
		GList *templist = NULL;
		templist = news_item_list;
		while (templist!=NULL)
		{
			fun_save_news_to_file ((NewsItem*)templist->data);
			templist = g_list_next (templist);
		}
	}

    	return;
}

/**
 * fun_backend_init:
 *
 * Initializes the fun news notification backend
 */
void
fun_news_backend_init (void)
{
	/* populate the existing list of news items
	 * stored on disk */
	fun_populate_existing_news_list ();

	return;
}

/**
 * fun_get_existing_news_list:
 *
 * Returns a GList of existing news items
 */
GList*
fun_get_existing_news_list (void)
{
	return e_news_item_list;
}

/**
 * fun_get_new_news_list:
 *
 * Returns a GList of existing news items
 */
GList*
fun_get_new_news_list (void)
{
	return news_item_list;
}

/**
 * fun_get_news_url_for_id:
 *
 * Returns a string representing the news URL (return value must not be freed)
 */
char*
fun_news_get_url_for_id (guint id)
{
	GList		*list = NULL;
	NewsItem 	*i = NULL;
	
	list = e_news_item_list;
	while (list != NULL)
	{
		i = NULL;
		i = list->data;
		if (id == i->id)
			break;
		list = list->next;
	}
	if (i != NULL)
		return g_strdup_printf ("http://frugalware.org/news/%d", i->id);

	return NULL;
}

