#ifndef __FUN_MESSAGES_H__
#define __FUN_MESSAGES_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>

void fun_error (const char *, const char *);

void fun_message (const char *, const char *);

gint fun_question (const char *, const char *);

#endif
