/*
 *  fun.c for fun
 *
 *  Copyright (C) 2007 by Priyank Gosalia <priyankmg@gmail.com>
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

#include <stdio.h>
#include <glib.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "fun.h"
#include "fun-config.h"

int
main (int argc, char **argv)
{
	/* set locale */
	setlocale (LC_ALL, "");
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
	
	/* initialize gtk library */
	gtk_init (&argc, &argv);
	
	/* initialize dbus and exit if init fails */
	if (fun_dbus_init() == FALSE)
		return 1;
	/* initialize fun configuration */
	fun_config_init ();
	/* initialize fun user interface */
	fun_ui_init ();

	gtk_main ();
	fun_ui_cleanup ();

	return 0;
}
