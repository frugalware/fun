#ifndef _FUN_CONFIG_H_
#define _FUN_CONFIG_H

#include <stdio.h>
#include <stdbool.h>
#include "wejpconfig.h"

/* Returns a GList of available browsers on the system */
GList* fun_config_get_available_browsers (void);

/* Returns a string representing the absolute path of a browser */
char* fun_config_get_browser_path (const char *name);

/* Returns true if file exists otherwise false */
bool fun_config_exists (void);

/* Parse config file and set the initial config values */
void fun_config_init (void);

/* Save current settings back to funrc */
void fun_config_save (void);

/* Read a value from funrc and return it as a string */
char * fun_config_get_value_string (const char *);

/* Read a value from funrc and return it as an int */
int fun_config_get_value_int (const char *);

/* Set a value for a particular key in funrc (int) */
void fun_config_set_value_int (const char *, int);

/* Set a value for a particular key in funrc (string) */
void fun_config_set_value_string (const char *, char *);

/* Free conf */
void fun_config_free (void);

#endif
