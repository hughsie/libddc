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
 * SECTION:libddc-control
 * @short_description: For managing different i2c controls
 *
 * A GObject to use for accessing controls.
 */

#include "config.h"

#include <stdlib.h>
#include <glib-object.h>

#include <libddc-device.h>
#include <libddc-control.h>

static void     libddc_control_finalize	(GObject     *object);

#define LIBDDC_CONTROL_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), LIBDDC_TYPE_CONTROL, LibddcControlPrivate))

/**
 * LibddcControlPrivate:
 *
 * Private #LibddcControl data
 **/
struct _LibddcControlPrivate
{
	guchar			 id;
	gboolean		 supported;
	LibddcDevice		*device;
	LibddcVerbose		 verbose;
	GArray			*values;
};

enum {
	PROP_0,
	PROP_SUPPORTED,
	PROP_LAST
};

G_DEFINE_TYPE (LibddcControl, libddc_control, G_TYPE_OBJECT)

#define LIBDDC_VCP_SET_DELAY_USECS   		50000

/**
 * libddc_control_get_description:
 **/
const gchar *
libddc_control_get_description (LibddcControl *control)
{
	g_return_val_if_fail (LIBDDC_IS_CONTROL(control), NULL);

	return libddc_get_vcp_description_from_index (control->priv->id);
}

/**
 * libddc_control_is_value_valid:
 **/
static gboolean
libddc_control_is_value_valid (LibddcControl *control, guint16 value, GError **error)
{
	guint i;
	gboolean ret = TRUE;
	GArray *array;
	GString *possible;

	/* no data */
	array = control->priv->values;
	if (array->len == 0)
		goto out;

	/* see if it is present in the description */
	for (i=0; i<array->len; i++) {
		ret = (g_array_index (array, guint16, i) == value);
		if (ret)
			goto out;
	}

	/* not found */
	if (!ret) {
		possible = g_string_new ("");
		for (i=0; i<array->len; i++)
			g_string_append_printf (possible, "%i ", g_array_index (array, guint16, i));
		g_set_error (error, LIBDDC_CONTROL_ERROR, LIBDDC_CONTROL_ERROR_FAILED,
			     "%i is not an allowed value for 0x%02x, possible values include %s",
			     value, control->priv->id, possible->str);
		g_string_free (possible, TRUE);
	}
out:
	return ret;
}

/**
 * libddc_control_set:
 *
 * write value to register ctrl of ddc/ci
 **/
gboolean
libddc_control_set (LibddcControl *control, guint16 value, GError **error)
{
	gboolean ret = FALSE;
	guchar buf[4];

	g_return_val_if_fail (LIBDDC_IS_CONTROL(control), FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	/* check this value is allowed */
	ret = libddc_control_is_value_valid (control, value, error);
	if (!ret)
		goto out;

	buf[0] = LIBDDC_VCP_SET;
	buf[1] = control->priv->id;
	buf[2] = (value >> 8);
	buf[3] = (value & 255);

	ret = libddc_device_write (control->priv->device, buf, sizeof(buf), error);
	if (!ret)
		goto out;

	/* Do the delay */
	g_usleep (LIBDDC_VCP_SET_DELAY_USECS);
out:
	return ret;
}

/**
 * libddc_control_reset:
 **/
gboolean
libddc_control_reset (LibddcControl *control, GError **error)
{
	gboolean ret;
	guchar buf[2];

	g_return_val_if_fail (LIBDDC_IS_CONTROL(control), FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	buf[0] = LIBDDC_VCP_RESET;
	buf[1] = control->priv->id;

	ret = libddc_device_write (control->priv->device, buf, sizeof(buf), error);
	if (!ret)
		goto out;

	/* Do the delay */
	g_usleep (LIBDDC_VCP_SET_DELAY_USECS);
out:
	return ret;
}

/**
 * libddc_control_request:
 **/
gboolean
libddc_control_request (LibddcControl *control, guint16 *value, guint16 *maximum, GError **error)
{
	gboolean ret = FALSE;
	guchar buf[8];
	gsize len;

	g_return_val_if_fail (LIBDDC_IS_CONTROL(control), FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	/* request data */
	buf[0] = LIBDDC_VCP_REQUEST;
	buf[1] = control->priv->id;
	if (!libddc_device_write (control->priv->device, buf, 2, error))
		goto out;

	/* get data */
	ret = libddc_device_read (control->priv->device, buf, 8, &len, error);
	if (!ret)
		goto out;

	/* check we got enough data */
	if (len != sizeof(buf)) {
		g_set_error (error, LIBDDC_CONTROL_ERROR, LIBDDC_CONTROL_ERROR_FAILED,
			     "Failed to parse control 0x%02x as incorrect length", control->priv->id);
		ret = FALSE;
		goto out;
	}

	/* message type incorrect */
	if (buf[0] != LIBDDC_VCP_REPLY) {
		g_set_error (error, LIBDDC_CONTROL_ERROR, LIBDDC_CONTROL_ERROR_FAILED,
			     "Failed to parse control 0x%02x as incorrect command returned", control->priv->id);
		ret = FALSE;
		goto out;
	}

	/* ensure the control is supported by the display */
	if (buf[1] != 0) {
		g_set_error (error, LIBDDC_CONTROL_ERROR, LIBDDC_CONTROL_ERROR_FAILED,
			     "Failed to parse control 0x%02x as unsupported", control->priv->id);
		ret = FALSE;
		goto out;
	}

	/* check we are getting the correct control */
	if (buf[2] != control->priv->id) {
		g_set_error (error, LIBDDC_CONTROL_ERROR, LIBDDC_CONTROL_ERROR_FAILED,
			     "Failed to parse control 0x%02x as incorrect id returned", control->priv->id);
		ret = FALSE;
		goto out;
	}

	if (value != NULL)
		*value = buf[6] * 256 + buf[7];
	if (maximum != NULL)
		*maximum = buf[4] * 256 + buf[5];
out:
	return ret;
}

/**
 * libddc_control_run:
 **/
gboolean
libddc_control_run (LibddcControl *control, GError **error)
{
	guchar buf[1];

	g_return_val_if_fail (LIBDDC_IS_CONTROL(control), FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	buf[0] = control->priv->id;
	return libddc_device_write (control->priv->device, buf, sizeof(buf), error);
}

/**
 * libddc_control_parse:
 **/
void
libddc_control_parse (LibddcControl *control, guchar id, const gchar *values)
{
	guint i;
	guint16 value;
	gchar **split = NULL;

	g_return_if_fail (LIBDDC_IS_CONTROL(control));

	/* just save this */
	control->priv->id = id;
	if (control->priv->verbose == LIBDDC_VERBOSE_OVERVIEW)
		g_debug ("add control 0x%02x (%s)", id, libddc_control_get_description (control));

	/* do we have any values to parse */
	if (values == NULL)
		goto out;

	/* tokenize */
	split = g_strsplit (values, " ", -1);
	for (i=0; split[i] != NULL; i++) {
		value = atoi (split[i]);
		if (control->priv->verbose == LIBDDC_VERBOSE_OVERVIEW)
			g_debug ("add value %i to control 0x%02x", value, id);
		g_array_append_val (control->priv->values, value);
	}
out:
	g_strfreev (split);
}

/**
 * libddc_control_set_device:
 **/
void
libddc_control_set_device (LibddcControl *control, LibddcDevice *device)
{
	g_return_if_fail (LIBDDC_IS_CONTROL(control));
	g_return_if_fail (LIBDDC_IS_DEVICE(device));
	control->priv->device = g_object_ref (device);
}

/**
 * libddc_control_get_id:
 **/
guchar
libddc_control_get_id (LibddcControl *control)
{
	g_return_val_if_fail (LIBDDC_IS_CONTROL(control), 0);

	return control->priv->id;
}

/**
 * libddc_control_get_values:
 *
 * Return value: an array of guint16
 **/
GArray *
libddc_control_get_values (LibddcControl *control)
{
	g_return_val_if_fail (LIBDDC_IS_CONTROL(control), NULL);

	return g_array_ref (control->priv->values);
}

/**
 * libddc_control_set_verbose:
 **/
void
libddc_control_set_verbose (LibddcControl *control, LibddcVerbose verbose)
{
	g_return_if_fail (LIBDDC_IS_CONTROL(control));
	control->priv->verbose = verbose;
}

/**
 * libddc_control_error_quark:
 *
 * Return value: Our personal error quark.
 *
 * Since: 0.0.1
 **/
GQuark
libddc_control_error_quark (void)
{
	static GQuark quark = 0;
	if (!quark)
		quark = g_quark_from_static_string ("libddc_control_error");
	return quark;
}

/**
 * libddc_control_get_property:
 **/
static void
libddc_control_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	LibddcControl *control = LIBDDC_CONTROL (object);
	LibddcControlPrivate *priv = control->priv;

	switch (prop_id) {
	case PROP_SUPPORTED:
		g_value_set_boolean (value, priv->supported);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

/**
 * libddc_control_set_property:
 **/
static void
libddc_control_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	switch (prop_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

/**
 * libddc_control_class_init:
 **/
static void
libddc_control_class_init (LibddcControlClass *klass)
{
	GParamSpec *pspec;
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = libddc_control_finalize;
	object_class->get_property = libddc_control_get_property;
	object_class->set_property = libddc_control_set_property;

	/**
	 * LibddcControl:supported:
	 *
	 * Since: 0.0.1
	 */
	pspec = g_param_spec_boolean ("supported", NULL, "if there are no transactions in progress on this control",
				      TRUE,
				      G_PARAM_READABLE);
	g_object_class_install_property (object_class, PROP_SUPPORTED, pspec);

	g_type_class_add_private (klass, sizeof (LibddcControlPrivate));
}

/**
 * libddc_control_init:
 **/
static void
libddc_control_init (LibddcControl *control)
{
	control->priv = LIBDDC_CONTROL_GET_PRIVATE (control);
	control->priv->id = 0xff;
	control->priv->values = g_array_new (FALSE, FALSE, sizeof(guint16));
}

/**
 * libddc_control_finalize:
 **/
static void
libddc_control_finalize (GObject *object)
{
	LibddcControl *control = LIBDDC_CONTROL (object);
	LibddcControlPrivate *priv = control->priv;

	g_return_if_fail (LIBDDC_IS_CONTROL(control));

	g_array_free (priv->values, TRUE);
	if (priv->device != NULL)
		g_object_unref (priv->device);

	G_OBJECT_CLASS (libddc_control_parent_class)->finalize (object);
}

/**
 * libddc_control_new:
 *
 * LibddcControl is a nice GObject wrapper for libddc.
 *
 * Return value: A new %LibddcControl instance
 *
 * Since: 0.0.1
 **/
LibddcControl *
libddc_control_new (void)
{
	LibddcControl *control;
	control = g_object_new (LIBDDC_TYPE_CONTROL, NULL);
	return LIBDDC_CONTROL (control);
}
