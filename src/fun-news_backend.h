#ifndef __FUN_NEWS_BACKEND_H__
#define __FUN_NEWS_BACKEND_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

/* the News item structure */
typedef struct _newsitem {
	guint	id;
	char	title[256];
	char	*description;
	char	date[256];
} NewsItem;

void fun_news_backend_init (void);

void fun_populate_existing_news_list (void);

GList* fun_get_existing_news_list (void);

GList* fun_get_new_news_list (void);

void fun_fetch_news_xml (void);

int fun_save_news_to_file (NewsItem *item);

GList* fun_compare_lists (GList *oldlist, GList *newlist);

char* fun_news_get_url_for_id (guint id);

#endif

