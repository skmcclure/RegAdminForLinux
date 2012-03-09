/*
 * Authors:	Sean Loaring
 *		Petter Nordahl-Hagen
 *              James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi Inc
 * Copyright (c) 2005-2006 Sean Loaring
 * Copyright (c) 1997-2004 Petter Nordahl-Hagen
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

#ifndef RREGADMIN_HIVE_UTIL_H
#define RREGADMIN_HIVE_UTIL_H 1

#include <stdio.h>

#include <rregadmin/util/path.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/hive/defines.h>
#include <rregadmin/hive/types.h>

G_BEGIN_DECLS

/** Take a path and get the KeyCell at the end of it.
 *
 * @ingroup hive_group
 */
KeyCell * key_cell_trav_path (Hive *hdesc, KeyCell *nk, const char *in_path);

/** Take a path and get the ValueKeyCell at the end of it.
 *
 * @ingroup hive_group
 */
ValueKeyCell *value_key_cell_trav_path (Hive *hdesc, KeyCell *nk,
                                        const char *path,
                                        const char *in_value_name);

/** Take a path and get the KeyCell at the end of it.
 *
 * This takes a rra_path and an option to create the key if it doesn't exist.
 *
 * @internal
 *
 * @ingroup hive_group
 */
KeyCell* kc_trav_path_internal(KeyCell *in_kc, rra_path *in_path,
                               gboolean in_create);

/** Take a KeyCell and generate the path back to the beginning of the hive.
 *
 * @ingroup hive_group
 */
rra_path* generate_key_cell_path(KeyCell *in_kc);

G_END_DECLS

#endif // RREGADMIN_HIVE_UTIL_H
