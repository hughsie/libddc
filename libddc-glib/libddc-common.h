/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2010 Richard Hughes <richard@hughsie.com>
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

#if !defined (__LIBDDC_H_INSIDE__) && !defined (LIBDDC_COMPILATION)
#error "Only <libddc.h> can be included directly."
#endif

#ifndef __LIBDDC_COMMON_H__
#define __LIBDDC_COMMON_H__

#define __LIBDDC_COMMON_H_INSIDE__

typedef enum {
	LIBDDC_VERBOSE_NONE,
	LIBDDC_VERBOSE_OVERVIEW,
	LIBDDC_VERBOSE_PROTOCOL
} LibddcVerbose;

#define LIBDDC_VCP_REQUEST			0x01
#define LIBDDC_VCP_REPLY			0x02
#define LIBDDC_VCP_SET				0x03
#define LIBDDC_VCP_RESET			0x09
#define LIBDDC_DEFAULT_DDCCI_ADDR		0x37
#define LIBDDC_DEFAULT_EDID_ADDR		0x50
#define LIBDDC_CAPABILITIES_REQUEST		0xf3
#define LIBDDC_CAPABILITIES_REPLY		0xe3
#define LIBDDC_COMMAND_PRESENCE			0xf7
#define LIBDDC_ENABLE_APPLICATION_REPORT	0xf5

#define	LIBDDC_VCP_ID_INVALID			0x00

#define LIBDDC_CTRL_DISABLE			0x0000
#define LIBDDC_CTRL_ENABLE			0x0001

const gchar	*libddc_get_vcp_description_from_index	(guchar		 idx);
guchar		 libddc_get_vcp_index_from_description	(const gchar	*description);

#undef __LIBDDC_COMMON_H_INSIDE__

#endif /* __LIBDDC_COMMON_H__ */

