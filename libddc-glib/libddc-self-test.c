/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2007-2010 Richard Hughes <richard@hughsie.com>
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "config.h"

#include <glib-object.h>

#include "libddc-client.h"
#include "libddc-device.h"

static void
libddc_test_device_func (void)
{
	LibddcDevice *device;

	device = libddc_device_new ();
	g_assert (device != NULL);

	g_object_unref (device);
}

static void
libddc_test_client_func (void)
{
	LibddcClient *client;

	client = libddc_client_new ();
	g_assert (client != NULL);

	g_object_unref (client);
}

int
main (int argc, char **argv)
{
	g_type_init ();

	g_test_init (&argc, &argv, NULL);

	/* tests go here */
	g_test_add_func ("/libddc-glib/device", libddc_test_device_func);
	g_test_add_func ("/libddc-glib/client", libddc_test_client_func);

	return g_test_run ();
}

