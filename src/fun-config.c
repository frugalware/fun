/*
 *  fun-config.c for fun
 *
 *  Copyright (C) 2007-2008 by Priyank Gosalia <priyankmg@gmail.com>
 *  Portions of this code are borrowed from gimmix
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

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <gtk/gtk.h>
#include "fun-config.h"

#define CONFIG_FILE			".funrc"
#define UPDATE_INTERVAL_DEFAULT		"60"
#define NEWS_INTERVAL_DEFAULT		"30"
#define NOTIFICATION_TIMEOUT_DEFAULT	"5"
#define GFPM_LAUNCHER_DEFAULT		"sudo"

static ConfigFile conf;

static gchar *browserlist[] = 	{	"Firefox",	"/usr/bin/firefox",
					"Epiphany",	"/usr/bin/epiphany",
					"Opera",	"/usr/bin/opera",
					"Konqueror",	"/usr/bin/konqueror",
					NULL
				};

GList*
fun_config_get_available_browsers (void)
{
	guint	i = 0;
	GList	*ret = NULL;
	
	while (browserlist[i] != NULL)
	{
		if (g_file_test(browserlist[++i],G_FILE_TEST_EXISTS))
			ret = g_list_append (ret, g_strdup(browserlist[--i]));
		i+=2;
	}
	
	return ret;
}

char*
fun_config_get_browser_path (const char *name)
{
	char	*ret = NULL;
	guint	i = 0;

	while (browserlist[i] != NULL)
	{
		if (!strcmp(browserlist[0],name))
		{
			ret = g_strdup (browserlist[++i]);
			break;
		}
		i+=2;
	}

	return ret;
}

void
fun_config_init (void)
{
	char	*rcfile = NULL;
	gchar	*browser = NULL;
	GList	*browsers = NULL;

	cfg_init_config_file_struct (&conf);
	cfg_add_key (&conf, "update_interval", "60");
	cfg_add_key (&conf, "notification_timeout", "5");
	cfg_add_key (&conf, "gfpm_launcher", "sudo");
	cfg_add_key (&conf, "news_enabled", "true");
	cfg_add_key (&conf, "news_interval", "30");

	/* set the default browser */
	browsers = fun_config_get_available_browsers ();
	if (browsers != NULL)
	{
		browser = g_strdup_printf (browsers->data);
		cfg_add_key (&conf, "news_browser", browser);
		g_list_free (browsers);
	}
	rcfile = cfg_get_path_to_config_file (CONFIG_FILE);
	if (cfg_read_config_file (&conf, rcfile) != 0)
	{
		goto cleanup;
	}
	else
	{
		if (!fun_config_get_value_int("update_interval"))
			cfg_add_key (&conf, "update_interval", UPDATE_INTERVAL_DEFAULT);
		if (!fun_config_get_value_int("news_interval"))
			cfg_add_key (&conf, "news_interval", NEWS_INTERVAL_DEFAULT);
		if (!fun_config_get_value_int("notification_timeout"))
			cfg_add_key (&conf, "notification_timeout", NOTIFICATION_TIMEOUT_DEFAULT);
		if (!fun_config_get_value_string("gfpm_launcher"))
			cfg_add_key (&conf, "gfpm_launcher", GFPM_LAUNCHER_DEFAULT);
		if (!fun_config_get_value_string("news_browser") && browser!=NULL)
			cfg_add_key (&conf, "news_browser", browser);	
	}
	cleanup:
	fun_config_save ();
	g_free (rcfile);
	g_free (browser);

	return;
}

char *
fun_config_get_value_string (const char *key)
{
	char *ret = NULL;

	ret = cfg_get_key_value (conf, (char*)key);

	return ret;
}

int
fun_config_get_value_int (const char *key)
{
	int ret = -1;

	ret = atoi (cfg_get_key_value (conf, (char*)key));

	return ret;
}

void
fun_config_set_value_string (const char *key, char *value)
{
	char *val = g_strdup_printf ("%d", value);
	cfg_add_key (&conf, key, value);
	g_free (val);

	return;
}

void
fun_config_set_value_int (const char *key, int value)
{
	char *val = g_strdup_printf ("%d", value);
	cfg_add_key (&conf, key, val);
	g_free (val);
	
	return;
}

void
fun_config_save (void)
{
	char *rcfile;
	
	rcfile = cfg_get_path_to_config_file (CONFIG_FILE);
	cfg_write_config_file (&conf, rcfile);
	chmod (rcfile, S_IRUSR|S_IWUSR);
	g_free (rcfile);
	
	return;
}

bool
fun_config_exists (void)
{
	char *config_file = NULL;
	bool status;
	
	config_file = cfg_get_path_to_config_file (CONFIG_FILE);
	if (g_file_test(config_file, G_FILE_TEST_EXISTS))
		status = true;
	else
		status = false;

	free (config_file);
	return status;
}

void
fun_config_free (void)
{
	cfg_free_config_file_struct (&conf);
	
	return;
}

