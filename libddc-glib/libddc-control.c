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

#define LIBDDC_VCP_REQUEST		0x01
#define LIBDDC_VCP_REPLY		0x02
#define LIBDDC_VCP_SET			0x03
#define LIBDDC_VCP_RESET		0x09
#define LIBDDC_VCP_SET_DELAY_USECS   	50000

typedef struct {
	guchar		 index;
	const gchar	*shortname;
} LibddcDescription;

static const LibddcDescription vcp_descriptions[] = {
	{ 0x00, "degauss" },
	{ 0x01,	"degauss" },
	{ 0x02,	"secondary-degauss" },
	{ 0x04,	"reset-factory-defaults" },
	{ 0x05,	"reset-brightness-and-contrast" },
	{ 0x06,	"reset-factory-geometry" },
	{ 0x08,	"reset-factory-default-color" },
	{ 0x0a,	"reset-factory-default-position" },
	{ 0x0c,	"reset-factory-default-size" },
	{ 0x0e, "image-lock-coarse" },
	{ 0x10, "brightness" },
	{ 0x12, "contrast" },
	{ 0x13, "backlight" },
	{ 0x14,	"select-color-preset" },
	{ 0x16,	"red-video-gain" },
	{ 0x18,	"green-video-gain" },
	{ 0x1a,	"blue-video-gain" },
	{ 0x1c, "hue" },
	{ 0x1e,	"auto-size-center" },
	{ 0x20,	"horizontal-position" },
	{ 0x22,	"horizontal-size" },
	{ 0x24,	"horizontal-pincushion" },
	{ 0x26,	"horizontal-pincushion-balance" },
	{ 0x28,	"horizontal-misconvergence" },
	{ 0x2a,	"horizontal-linearity" },
	{ 0x2c,	"horizontal-linearity-balance" },
	{ 0x30,	"vertical-position" },
	{ 0x32,	"vertical-size" },
	{ 0x34,	"vertical-pincushion" },
	{ 0x36,	"vertical-pincushion-balance" },
	{ 0x38,	"vertical-misconvergence" },
	{ 0x3a,	"vertical-linearity" },
	{ 0x3c,	"vertical-linearity-balance" },
	{ 0x3e,	"image-lock-fine" },
	{ 0x40,	"parallelogram-distortion" },
	{ 0x42,	"trapezoidal-distortion" },
	{ 0x44, "tilt" },
	{ 0x46,	"top-corner-distortion-control" },
	{ 0x48,	"top-corner-distortion-balance" },
	{ 0x4a,	"bottom-corner-distortion-control" },
	{ 0x4c,	"bottom-corner-distortion-balance" },
	{ 0x50,	"hue" },
	{ 0x52,	"saturation" },
	{ 0x54, "color-temp" },
	{ 0x56,	"horizontal-moire" },
	{ 0x58,	"vertical-moire" },
	{ 0x5a, "auto-size" },
	{ 0x5c,	"landing-adjust" },
	{ 0x5e,	"input-level-select" },
	{ 0x60,	"input-source-select" },
	{ 0x62,	"audio-speaker-volume-adjust" },
	{ 0x64,	"audio-microphone-volume-adjust" },
	{ 0x66,	"on-screen-displa" },
	{ 0x68,	"language-select" },
	{ 0x6c,	"red-video-black-level" },
	{ 0x6e,	"green-video-black-level" },
	{ 0x70,	"blue-video-black-level" },
	{ 0x8c, "sharpness" },
	{ 0x94, "mute" },
	{ 0xa2,	"auto-size-center" },
	{ 0xa4,	"polarity-horizontal-synchronization" },
	{ 0xa6,	"polarity-vertical-synchronization" },
	{ 0xa8,	"synchronization-type" },
	{ 0xaa,	"screen-orientation" },
	{ 0xac,	"horizontal-frequency" },
	{ 0xae,	"vertical-frequency" },
	{ 0xb0,	"settings" },
	{ 0xca,	"on-screen-display" },
	{ 0xcc,	"samsung-on-screen-display-language" },
	{ 0xc9,	"firmware-version" },
	{ 0xd4,	"stereo-mode" },
	{ 0xd6,	"dpms-control-(1---on/4---stby)" },
	{ 0xdc,	"magicbright-(1---text/2---internet/3---entertain/4---custom)" },
	{ 0xdf,	"vcp-version" },
	{ 0xe0,	"samsung-color-preset-(0---normal/1---warm/2---cool)" },
	{ 0xe1,	"power-control-(0---off/1---on)" },
	{ 0xe2, "auto-source" },
	{ 0xe8, "tl-purity" },
	{ 0xe9, "tr-purity" },
	{ 0xea, "bl-purity" },
	{ 0xeb, "br-purity" },
	{ 0xed,	"samsung-red-video-black-level" },
	{ 0xee,	"samsung-green-video-black-level" },
	{ 0xef,	"samsung-blue-video-black-level" },
	{ 0xf0, "magic-color" },
	{ 0xf1, "fe-brightness" },
	{ 0xf2, "fe-clarity / gamma" },
	{ 0xf3, "fe-color" },
	{ 0xf5, "samsung-osd" },
	{ 0xf6, "resolutionnotifier" },
	{ 0xf9, "super-bright" },
	{ 0xfc, "fe-mode" },
	{ 0xfd, "power-led" },
	{ 0xff, NULL }};

/**
 * libddc_control_get_description:
 **/
const gchar *
libddc_control_get_description (LibddcControl *control)
{
	guint i;

	g_return_val_if_fail (LIBDDC_IS_CONTROL(control), NULL);

	for (i=0;;i++) {
		if (vcp_descriptions[i].index == control->priv->id ||
		    vcp_descriptions[i].index == 0xff)
			break;
	}
	return vcp_descriptions[i].shortname;
}

/**
 * libddc_control_is_value_valid:
 **/
static gboolean
libddc_control_is_value_valid (LibddcControl *control, guint16 value)
{
	guint i;
	gboolean ret = TRUE;
	GArray *array;

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
	ret = libddc_control_is_value_valid (control, value);
	if (!ret) {
		g_set_error (error, LIBDDC_CONTROL_ERROR, LIBDDC_CONTROL_ERROR_FAILED,
			     "%i is not an allowed value for 0x%02x", value, control->priv->id);
		goto out;
	}

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
	guchar value;
	gchar **split = NULL;

	g_return_if_fail (LIBDDC_IS_CONTROL(control));

	/* just save this */
	control->priv->id = id;
	g_debug ("add control 0x%02x (%s)", id, libddc_control_get_description (control));

	/* do we have any values to parse */
	if (values == NULL)
		goto out;

	/* tokenize */
	split = g_strsplit (values, " ", -1);
	for (i=0; split[i] != NULL; i++) {
		value = atoi (split[i]);
		g_debug ("add value %i to control 0x%02x", id, value);
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
