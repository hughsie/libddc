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

#ifndef __LIBDDC_CLIENT_H
#define __LIBDDC_CLIENT_H

#include <glib-object.h>
#include <gio/gio.h>

#include <libddc-common.h>
#include <libddc-device.h>
#include <libddc-client.h>

G_BEGIN_DECLS

#define LIBDDC_TYPE_CLIENT		(libddc_client_get_type ())
#define LIBDDC_CLIENT(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), LIBDDC_TYPE_CLIENT, LibddcClient))
#define LIBDDC_CLIENT_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), LIBDDC_TYPE_CLIENT, LibddcClientClass))
#define LIBDDC_IS_CLIENT(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), LIBDDC_TYPE_CLIENT))
#define LIBDDC_IS_CLIENT_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), LIBDDC_TYPE_CLIENT))
#define LIBDDC_CLIENT_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), LIBDDC_TYPE_CLIENT, LibddcClientClass))
#define LIBDDC_CLIENT_ERROR		(libddc_client_error_quark ())
#define LIBDDC_CLIENT_TYPE_ERROR	(libddc_client_error_get_type ())

/**
 * LibddcClientError:
 * @LIBDDC_CLIENT_ERROR_FAILED: the transaction failed for an unknown reason
 *
 * Errors that can be thrown
 */
typedef enum
{
	LIBDDC_CLIENT_ERROR_FAILED
} LibddcClientError;

typedef struct _LibddcClientPrivate		LibddcClientPrivate;
typedef struct _LibddcClient			LibddcClient;
typedef struct _LibddcClientClass		LibddcClientClass;

struct _LibddcClient
{
	 GObject		 parent;
	 LibddcClientPrivate	*priv;
};

struct _LibddcClientClass
{
	GObjectClass	parent_class;

	/* signals */
	void		(* changed)			(LibddcClient	*client);
	/* padding for future expansion */
	void (*_libddc_reserved1) (void);
	void (*_libddc_reserved2) (void);
	void (*_libddc_reserved3) (void);
	void (*_libddc_reserved4) (void);
	void (*_libddc_reserved5) (void);
};

GQuark		 libddc_client_error_quark		(void);
GType		 libddc_client_get_type		  	(void);
LibddcClient	*libddc_client_new			(void);

gboolean	 libddc_client_close			(LibddcClient		*client,
							 GError			**error);
GPtrArray	*libddc_client_get_devices		(LibddcClient		*client,
							 GError			**error);
LibddcDevice	*libddc_client_get_device_from_edid	(LibddcClient		*client,
							 const gchar		*edid_md5,
							 GError			**error);
void		 libddc_client_set_verbose		(LibddcClient		*client,
							 LibddcVerbose		 verbose);

G_END_DECLS

#endif /* __LIBDDC_CLIENT_H */

