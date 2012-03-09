/*
 * Authors:     Sean Loaring
 *              James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi Inc
 * Copyright (c) 2005-2006 Sean Loaring
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef RREGADMIN_REGISTRY_TYPES_H
#define RREGADMIN_REGISTRY_TYPES_H 1

#include <glib.h>

#include <rregadmin/util/macros.h>
#include <rregadmin/hive/types.h>

G_BEGIN_DECLS

typedef struct RegistryKey_ RegistryKey;

typedef struct Registry_ Registry;

typedef struct hive_info_ hive_info;

typedef struct RegKey_ RegKey;
typedef struct RegValue_ RegValue;

/** Type representing a registry key id.
 *
 * @ingroup registry_group
 */
typedef gint32 reg_id;

/** Type representing a hive id.
 *
 * @ingroup registry_group
 */
typedef gint32 hive_id;

G_END_DECLS

#endif // RREGADMIN_REGISTRY_TYPES_H
