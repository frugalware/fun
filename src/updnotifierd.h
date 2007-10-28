/***************************************************************************
 *  netconfigd.h
 *  Author(s):  Alex Smith <alex@alex-smith.me.uk>
 *  Copyright (C) 2007 Frugalware Developer Team
 ****************************************************************************/

/*
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

#ifndef _UPDNOTIFIERD_H
#define _UPDNOTIFIERD_H

typedef struct
{
	GObject parent;
	DBusGConnection *connection;
} UpdNotifier;

typedef struct
{
	GObjectClass parent_class;
} UpdNotifierClass;

static void updnotifierd_init(UpdNotifier *server);
static void updnotifierd_class_init(UpdNotifierClass *class);

gboolean updnotifier_update_database(UpdNotifier *obj, gchar **packages, GError **error);
gboolean updnotifier_test_service(UpdNotifier *obj, gint *ret, GError **error);

#endif
