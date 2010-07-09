/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2010 Richard Hughes <richard@hughsie.com>
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

/**
 * SECTION:libddc-client
 * @short_description: For managing different i2c devices
 *
 * A GObject to use for accessing devices.
 */

#include "config.h"

#include <glib-object.h>
#include <stdlib.h>

#include <libddc-client.h>
#include <libddc-device.h>

static void     libddc_client_finalize	(GObject     *object);

#define LIBDDC_CLIENT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), LIBDDC_TYPE_CLIENT, LibddcClientPrivate))

/**
 * LibddcClientPrivate:
 *
 * Private #LibddcClient data
 **/
struct _LibddcClientPrivate
{
	GPtrArray		*devices;
	gboolean		 has_coldplug;
	LibddcVerbose		 verbose;
};

enum {
	PROP_0,
	PROP_HAS_COLDPLUG,
	PROP_LAST
};

G_DEFINE_TYPE (LibddcClient, libddc_client, G_TYPE_OBJECT)

/**
 * libddc_client_ensure_coldplug:
 **/
static gboolean
libddc_client_ensure_coldplug (LibddcClient *client, GError **error)
{
	gboolean ret = FALSE;
	gboolean any_found = FALSE;
	guint i;
	gchar *filename;
	GError *error_local = NULL;
	LibddcDevice *device;

	g_return_val_if_fail (LIBDDC_IS_CLIENT(client), FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	/* already done */
	if (client->priv->has_coldplug)
		return TRUE;

	/* ensure we have the module loaded */
	ret = g_file_test ("/sys/module/i2c_dev/srcversion", G_FILE_TEST_EXISTS);
	if (!ret) {
		g_set_error_literal (error, LIBDDC_CLIENT_ERROR, LIBDDC_CLIENT_ERROR_FAILED,
				     "unable to use I2C, you need to 'modprobe i2c-dev'");
		goto out;
	}

	/* try each i2c port */
	for (i=0; i<16; i++) {
		filename = g_strdup_printf ("/dev/i2c-%i", i);
		if (!g_file_test (filename, G_FILE_TEST_EXISTS)) {
			g_free (filename);
			break;
		}
		device = libddc_device_new ();
		libddc_device_set_verbose (device, client->priv->verbose);
		ret = libddc_device_open (device, filename, &error_local);
		if (!ret) {
			if (client->priv->verbose == LIBDDC_VERBOSE_OVERVIEW)
				g_warning ("failed to open %s: %s", filename, error_local->message);
			g_clear_error (&error_local);
		} else {
			if (client->priv->verbose == LIBDDC_VERBOSE_OVERVIEW)
				g_debug ("success, adding %s", filename);
			any_found = TRUE;
			g_ptr_array_add (client->priv->devices, g_object_ref (device));
		}
		g_object_unref (device);
		g_free (filename);
	}

	/* nothing found */
	if (!any_found) {
		g_set_error_literal (error, LIBDDC_CLIENT_ERROR, LIBDDC_CLIENT_ERROR_FAILED,
				     "No devices found");
		goto out;
	}

	/* success */
	client->priv->has_coldplug = TRUE;
out:
	return any_found;
}

/**
 * libddc_client_close:
 **/
gboolean
libddc_client_close (LibddcClient *client, GError **error)
{
	guint i;
	gboolean ret = TRUE;
	LibddcDevice *device;

	g_return_val_if_fail (LIBDDC_IS_CLIENT(client), FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	/* iterate each device */
	for (i=0; i<client->priv->devices->len; i++) {
		device = g_ptr_array_index (client->priv->devices, i);
		ret = libddc_device_close (device, error);
		if (!ret)
			goto out;
	}
out:
	return ret;
}

/**
 * libddc_client_get_devices:
 **/
GPtrArray *
libddc_client_get_devices (LibddcClient *client, GError **error)
{
	gboolean ret;
	GPtrArray *devices = NULL;

	g_return_val_if_fail (LIBDDC_IS_CLIENT(client), NULL);
	g_return_val_if_fail (error == NULL || *error == NULL, NULL);

	/* get capabilities */
	ret = libddc_client_ensure_coldplug (client, error);
	if (!ret)
		goto out;

	/* success */
	devices = g_ptr_array_ref (client->priv->devices);
out:
	return devices;
}

/**
 * libddc_client_get_device_from_edid:
 **/
LibddcDevice *
libddc_client_get_device_from_edid (LibddcClient *client, const gchar *edid_md5, GError **error)
{
	guint i;
	gboolean ret;
	const gchar *edid_md5_tmp;
	LibddcDevice *device = NULL;
	LibddcDevice *device_tmp;

	g_return_val_if_fail (LIBDDC_IS_CLIENT(client), NULL);
	g_return_val_if_fail (edid_md5 != NULL, NULL);
	g_return_val_if_fail (error == NULL || *error == NULL, NULL);

	/* get capabilities */
	ret = libddc_client_ensure_coldplug (client, error);
	if (!ret)
		goto out;

	/* iterate each device */
	for (i=0; i<client->priv->devices->len; i++) {
		device_tmp = g_ptr_array_index (client->priv->devices, i);

		/* get the md5 of the device */
		edid_md5_tmp = libddc_device_get_edid_md5 (device_tmp, error);
		if (edid_md5_tmp == NULL)
			goto out;

		/* matches? */
		if (g_strcmp0 (edid_md5, edid_md5_tmp) == 0) {
			device = device_tmp;
			break;
		}
	}

	/* failure */
	if (device == NULL) {
		g_set_error (error, LIBDDC_CLIENT_ERROR, LIBDDC_CLIENT_ERROR_FAILED,
			     "No devices found with edid %s", edid_md5);
	}
out:
	return device;
}

/**
 * libddc_client_set_verbose:
 **/
void
libddc_client_set_verbose (LibddcClient *client, LibddcVerbose verbose)
{
	g_return_if_fail (LIBDDC_IS_CLIENT(client));
	client->priv->verbose = verbose;
}

/**
 * libddc_client_error_quark:
 *
 * Return value: Our personal error quark.
 *
 * Since: 0.0.1
 **/
GQuark
libddc_client_error_quark (void)
{
	static GQuark quark = 0;
	if (!quark)
		quark = g_quark_from_static_string ("libddc_client_error");
	return quark;
}

/**
 * libddc_client_get_property:
 **/
static void
libddc_client_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	LibddcClient *client = LIBDDC_CLIENT (object);
	LibddcClientPrivate *priv = client->priv;

	switch (prop_id) {
	case PROP_HAS_COLDPLUG:
		g_value_set_boolean (value, priv->has_coldplug);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

/**
 * libddc_client_set_property:
 **/
static void
libddc_client_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	switch (prop_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

/**
 * libddc_client_class_init:
 **/
static void
libddc_client_class_init (LibddcClientClass *klass)
{
	GParamSpec *pspec;
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = libddc_client_finalize;
	object_class->get_property = libddc_client_get_property;
	object_class->set_property = libddc_client_set_property;

	/**
	 * LibddcClient:has-coldplug:
	 *
	 * Since: 0.0.1
	 */
	pspec = g_param_spec_boolean ("has-coldplug", NULL, "if there are no transactions in progress on this client",
				      TRUE,
				      G_PARAM_READABLE);
	g_object_class_install_property (object_class, PROP_HAS_COLDPLUG, pspec);

	g_type_class_add_private (klass, sizeof (LibddcClientPrivate));
}

/**
 * libddc_client_init:
 **/
static void
libddc_client_init (LibddcClient *client)
{
	client->priv = LIBDDC_CLIENT_GET_PRIVATE (client);
	client->priv->devices = g_ptr_array_new_with_free_func ((GDestroyNotify) g_object_unref);
}

/**
 * libddc_client_finalize:
 **/
static void
libddc_client_finalize (GObject *object)
{
	LibddcClient *client = LIBDDC_CLIENT (object);
	LibddcClientPrivate *priv = client->priv;

	g_return_if_fail (LIBDDC_IS_CLIENT(client));

	g_ptr_array_unref (priv->devices);
	G_OBJECT_CLASS (libddc_client_parent_class)->finalize (object);
}

/**
 * libddc_client_new:
 *
 * LibddcClient is a nice GObject wrapper for libddc.
 *
 * Return value: A new %LibddcClient instance
 *
 * Since: 0.0.1
 **/
LibddcClient *
libddc_client_new (void)
{
	LibddcClient *client;
	client = g_object_new (LIBDDC_TYPE_CLIENT, NULL);
	return LIBDDC_CLIENT (client);
}
