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

#if !defined (__LIBDDC_H_INSIDE__) && !defined (LIBDDC_COMPILATION)
#error "Only <libddc.h> can be included directly."
#endif

#ifndef __LIBDDC_DEVICE_H
#define __LIBDDC_DEVICE_H

#include <glib-object.h>
#include <gio/gio.h>

#include <libddc-common.h>
#include <libddc-device.h>

G_BEGIN_DECLS

#define LIBDDC_TYPE_DEVICE		(libddc_device_get_type ())
#define LIBDDC_DEVICE(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), LIBDDC_TYPE_DEVICE, LibddcDevice))
#define LIBDDC_DEVICE_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), LIBDDC_TYPE_DEVICE, LibddcDeviceClass))
#define LIBDDC_IS_DEVICE(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), LIBDDC_TYPE_DEVICE))
#define LIBDDC_IS_DEVICE_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), LIBDDC_TYPE_DEVICE))
#define LIBDDC_DEVICE_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), LIBDDC_TYPE_DEVICE, LibddcDeviceClass))
#define LIBDDC_DEVICE_ERROR		(libddc_device_error_quark ())
#define LIBDDC_DEVICE_TYPE_ERROR	(libddc_device_error_get_type ())

/**
 * LibddcDeviceError:
 * @LIBDDC_DEVICE_ERROR_FAILED: the transaction failed for an unknown reason
 *
 * Errors that can be thrown
 */
typedef enum
{
	LIBDDC_DEVICE_ERROR_FAILED
} LibddcDeviceError;

typedef struct _LibddcDevicePrivate		LibddcDevicePrivate;
typedef struct _LibddcDevice			LibddcDevice;
typedef struct _LibddcDeviceClass		LibddcDeviceClass;

struct _LibddcDevice
{
	 GObject		 parent;
	 LibddcDevicePrivate	*priv;
};

struct _LibddcDeviceClass
{
	GObjectClass	parent_class;

	/* signals */
	void		(* changed)			(LibddcDevice	*device);
	/* padding for future expansion */
	void (*_libddc_reserved1) (void);
	void (*_libddc_reserved2) (void);
	void (*_libddc_reserved3) (void);
	void (*_libddc_reserved4) (void);
	void (*_libddc_reserved5) (void);
};

typedef enum {
	LIBDDC_DEVICE_KIND_LCD,
	LIBDDC_DEVICE_KIND_CRT,
	LIBDDC_DEVICE_KIND_UNKNOWN
} LibddcDeviceKind;

/* incest */
#include <libddc-control.h>

GQuark		 libddc_device_error_quark		(void);
GType		 libddc_device_get_type		  	(void);
LibddcDevice	*libddc_device_new			(void);

gboolean	 libddc_device_open			(LibddcDevice	*device,
							 const gchar	*filename,
							 GError		**error);
gboolean	 libddc_device_close			(LibddcDevice	*device,
							 GError		**error);
const guint8	*libddc_device_get_edid			(LibddcDevice	*device,
							 gsize		*length,
							 GError		**error);
const gchar	*libddc_device_get_edid_md5		(LibddcDevice	*device,
							 GError		**error);
gboolean	 libddc_device_write			(LibddcDevice	*device,
							 guchar		 *data,
							 gsize		 length,
							 GError		**error);
gboolean	 libddc_device_read			(LibddcDevice	*device,
							 guchar		*data,
							 gsize		 data_length,
							 gsize		*recieved_length,
							 GError		**error);
gboolean	 libddc_device_save			(LibddcDevice	*device,
							 GError		**error);
const gchar	*libddc_device_get_pnpid		(LibddcDevice	*device,
							 GError		**error);
const gchar	*libddc_device_get_model		(LibddcDevice	*device,
							 GError		**error);
LibddcDeviceKind libddc_device_get_kind			(LibddcDevice	*device,
							 GError		**error);
GPtrArray	*libddc_device_get_controls		(LibddcDevice	*device,
							 GError		**error);
LibddcControl	*libddc_device_get_control_by_id	(LibddcDevice	*device,
							 guchar		 id,
							 GError		**error);
void		 libddc_device_set_verbose		(LibddcDevice	*device,
							 LibddcVerbose verbose);
const gchar	*libddc_device_get_vcp_description	(guint		 id);

G_END_DECLS

#endif /* __LIBDDC_DEVICE_H */

