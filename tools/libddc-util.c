/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2009-2010 Richard Hughes <richard@hughsie.com>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <config.h>
#include <glib/gstdio.h>

#include <libddc.h>

/**
 * show_device_cb:
 **/
static void
show_device_cb (LibddcDevice *device, gpointer user_data)
{
	guint i, j;
	gboolean ret;
	guint16 value, max;
	GPtrArray *array;
	LibddcControl *control;
	GArray *values;
	GError *error = NULL;
	const gchar *desc;

	desc = libddc_device_get_edid_md5 (device, &error);
	if (desc == NULL) {
		g_warning ("failed to get EDID: %s", error->message);
		g_error_free (error);
		goto out;
	}
	g_print ("EDID: \t%s\n", desc);

	desc = libddc_device_get_pnpid (device, &error);
	if (desc == NULL) {
		g_warning ("failed to get PNPID: %s", error->message);
		g_error_free (error);
		goto out;
	}
	g_print ("PNPID:\t%s\n", desc);

	desc = libddc_device_get_model (device, &error);
	if (desc == NULL) {
		g_warning ("failed to get model: %s", error->message);
		g_error_free (error);
		goto out;
	}
	g_print ("Model:\t%s\n", desc);

	array = libddc_device_get_controls (device, &error);
	if (array == NULL) {
		g_warning ("failed to get caps: %s", error->message);
		g_error_free (error);
		goto out;
	}
	for (i = 0; i < array->len; i++) {
		control = g_ptr_array_index (array, i);

		desc = libddc_control_get_description (control);
		g_print ("0x%02x\t[%s]", libddc_control_get_id (control), desc != NULL ? desc : "unknown");

		/* print allowed values */
		values = libddc_control_get_values (control);
		if (values->len > 0) {
			g_print ("\tValues:\n");
			for (j=0; j<values->len; j++) {
				g_print ("\tvalues: %i", g_array_index (values, guint16, j));
			}
			g_print ("\n");
		}
		g_array_unref (values);
	}
	g_ptr_array_unref (array);

	control = libddc_device_get_control_by_id (device, LIBDDC_CONTROL_ID_BRIGHTNESS, &error);
	if (control == NULL) {
		g_warning ("failed to get brightness cap: %s", error->message);
		g_error_free (error);
		goto out;
	}

	ret = libddc_control_read (control, &value, &max, &error);
	if (!ret) {
		g_warning ("failed to read: %s", error->message);
		g_error_free (error);
		goto out;
	}
	g_debug  ("brightness=%i, max=%i", value, max);

	ret = libddc_control_write (control, 0, &error);
	if (!ret) {
		g_warning ("failed to write: %s", error->message);
		g_error_free (error);
		goto out;
	}
	ret = libddc_control_write (control, value, &error);
	if (!ret) {
		g_warning ("failed to write: %s", error->message);
		g_error_free (error);
		goto out;
	}
	g_object_unref (control);
out:
	return;
}

/**
 * main:
 **/
int
main (int argc, char **argv)
{
	gboolean ret;
	LibddcClient *client;
	GError *error = NULL;
	GPtrArray *array;

	g_type_init ();

	client = libddc_client_new ();
	libddc_client_set_verbose (client, LIBDDC_VERBOSE_OVERVIEW);

	array = libddc_client_get_devices (client, &error);
	if (array == NULL) {
		g_warning ("failed to get device list: %s", error->message);
		g_error_free (error);
		goto out;
	}

	/* show device details */
	g_ptr_array_foreach (array, (GFunc) show_device_cb, NULL);
	g_ptr_array_unref (array);

	ret = libddc_client_close (client, &error);
	if (!ret) {
		g_warning ("failed to close: %s", error->message);
		g_error_free (error);
		goto out;
	}
	g_object_unref (client);
out:
	return 0;
}
