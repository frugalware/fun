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

#define CONFIG_FILE ".funrc"

static ConfigFile conf;

bool
fun_config_init (void)
{
	char *rcfile = NULL;

	cfg_init_config_file_struct (&conf);
	cfg_add_key (&conf, "update_interval", "60");
	cfg_add_key (&conf, "notification_timeout", "5");
	cfg_add_key (&conf, "gfpm_launcher", "sudo");
	rcfile = cfg_get_path_to_config_file (CONFIG_FILE);
	if (cfg_read_config_file (&conf, rcfile) != 0)
	{
		fun_config_save ();
		g_free (rcfile);
		return false;
	}
	else
	{
		g_free (rcfile);
		return true;
	}

	return false;
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

