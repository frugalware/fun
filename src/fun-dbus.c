/*
 *  fun-dbus.c for fun
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

#include "fun-dbus.h"

static DBusConnection	*fun_conn = NULL;

gboolean
fun_dbus_init (void)
{
	DBusError error;
	
	dbus_error_init (&error);
	fun_conn = dbus_bus_get (DBUS_BUS_SYSTEM, &error);
	if (fun_conn == NULL)
	{
		fprintf (stderr, "Failed to open connection to the bus\n");
		dbus_error_free (&error);
		return FALSE;
	}

	return TRUE;
}

gboolean
fun_dbus_perform_service (guint service)
{
	DBusMessage	*message = NULL;
	DBusMessage	*reply = NULL;
	DBusError	error;
	int		reply_timeout = -1;

	switch (service)
	{
		gchar **package_list = NULL;
		case PERFORM_UPDATE:
		{
			dbus_error_init (&error);
			message = dbus_message_new_method_call ("org.frugalware.UpdNotifier",
								"/org/frugalware/UpdNotifier",
								"org.frugalware.UpdNotifier",
								"PerformUpdate");
			reply = dbus_connection_send_with_reply_and_block (fun_conn, message, reply_timeout, &error);
			if (dbus_error_is_set(&error))
			{
				fprintf (stderr, "ERROR: %s\n", error.message);
				dbus_error_free (&error);
				return FALSE;
			}
			if (!dbus_message_get_args (reply, &error,
						DBUS_TYPE_STRING, &package_list,
						DBUS_TYPE_INVALID))
			{
				fprintf (stderr, "ERROR: %s\n", error.message);
				dbus_error_free (&error);
				return FALSE;
			}
			dbus_message_unref (reply);
			dbus_message_unref (message);
			break;
		}
		case TEST_SERVICE:
		{
			guint ret = 0;
			dbus_error_init (&error);
			message = dbus_message_new_method_call ("org.frugalware.UpdNotifier",
								"/org/frugalware/UpdNotifier",
								"org.frugalware.UpdNotifier",
								"TestService");
			reply = dbus_connection_send_with_reply_and_block (fun_conn, message, reply_timeout, &error);
			if (dbus_error_is_set(&error))
			{
				fprintf (stderr, "ERROR: %s\n", error.message);
				dbus_error_free (&error);
				return FALSE;
			}
			if (!dbus_message_get_args (reply, &error,
						DBUS_TYPE_INT32, &ret,
						DBUS_TYPE_INVALID))
			{
				fprintf (stderr, "ERROR: %s\n", error.message);
				dbus_error_free (&error);
				return FALSE;
			}
			dbus_message_unref (reply);
			dbus_message_unref (message);
			break;
		}
		
		default: break;
	}

	return TRUE;
}


