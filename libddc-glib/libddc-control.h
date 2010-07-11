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

#ifndef __LIBDDC_CONTROL_H
#define __LIBDDC_CONTROL_H

#include <glib-object.h>
#include <gio/gio.h>

#include <libddc-common.h>
#include <libddc-control.h>
#include <libddc-device.h>

G_BEGIN_DECLS

#define LIBDDC_TYPE_CONTROL		(libddc_control_get_type ())
#define LIBDDC_CONTROL(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), LIBDDC_TYPE_CONTROL, LibddcControl))
#define LIBDDC_CONTROL_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), LIBDDC_TYPE_CONTROL, LibddcControlClass))
#define LIBDDC_IS_CONTROL(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), LIBDDC_TYPE_CONTROL))
#define LIBDDC_IS_CONTROL_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), LIBDDC_TYPE_CONTROL))
#define LIBDDC_CONTROL_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), LIBDDC_TYPE_CONTROL, LibddcControlClass))
#define LIBDDC_CONTROL_ERROR		(libddc_control_error_quark ())
#define LIBDDC_CONTROL_TYPE_ERROR	(libddc_control_error_get_type ())

/**
 * LibddcControlError:
 * @LIBDDC_CONTROL_ERROR_FAILED: the transaction failed for an unknown reason
 *
 * Errors that can be thrown
 */
typedef enum
{
	LIBDDC_CONTROL_ERROR_FAILED
} LibddcControlError;

typedef struct _LibddcControlPrivate		LibddcControlPrivate;
typedef struct _LibddcControl			LibddcControl;
typedef struct _LibddcControlClass		LibddcControlClass;

struct _LibddcControl
{
	 GObject		 parent;
	 LibddcControlPrivate	*priv;
};

struct _LibddcControlClass
{
	GObjectClass	parent_class;
	/* padding for future expansion */
	void (*_libddc_reserved1) (void);
	void (*_libddc_reserved2) (void);
	void (*_libddc_reserved3) (void);
	void (*_libddc_reserved4) (void);
	void (*_libddc_reserved5) (void);
};

typedef struct {
	guint			 id;
	GPtrArray		*int_values;
} LibddcControlCap;

/* control numbers */
#define LIBDDC_CONTROL_ID_BRIGHTNESS			0x10

GQuark		 libddc_control_error_quark		(void);
GType		 libddc_control_get_type		(void);
LibddcControl	*libddc_control_new			(void);

void		 libddc_control_parse			(LibddcControl	*control,
							 guchar		 id,
							 const gchar	*values);
void		 libddc_control_set_device		(LibddcControl	*control,
							 LibddcDevice	*device);
void		 libddc_control_set_verbose		(LibddcControl	*control,
							 LibddcVerbose verbose);
gboolean	 libddc_control_run			(LibddcControl	*control,
							 GError		**error);
gboolean	 libddc_control_request			(LibddcControl	*control,
							 guint16	*value,
							 guint16	*maximum,
							 GError		**error);
gboolean	 libddc_control_set			(LibddcControl	*control,
							 guint16	 value,
							 GError		**error);
gboolean	 libddc_control_reset			(LibddcControl	*control,
							 GError		**error);
guchar		 libddc_control_get_id			(LibddcControl	*control);
const gchar	*libddc_control_get_description		(LibddcControl	*control);
GArray		*libddc_control_get_values		(LibddcControl	*control);

G_END_DECLS

#endif /* __LIBDDC_CONTROL_H */

