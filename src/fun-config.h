#ifndef _FUN_CONFIG_H_
#define _FUN_CONFIG_H

#include <stdio.h>
#include <stdbool.h>
#include "wejpconfig.h"

/* Returns true if file exists otherwise false */
bool fun_config_exists (void);

/* Parse config file and set the initial config values */
bool fun_config_init (void);

/* Save current settings back to funrc */
void fun_config_save (void);

/* Read a value from funrc and return it as a string */
char * fun_config_get_value_string (const char *);

/* Read a value from funrc and return it as an int */
int fun_config_get_value_int (const char *);

/* Set a value for a particular key in funrc */
void fun_config_set_value_int (const char *, int);

/* Free conf */
void fun_config_free (void);

#endif
