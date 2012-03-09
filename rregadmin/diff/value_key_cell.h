/*
 * Authors:     James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006,2008 Racemi Inc
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

#ifndef RREGADMIN_DIFF_VALUE_KEY_CELL_H
#define RREGADMIN_DIFF_VALUE_KEY_CELL_H 1

#include <glib.h>

#include <rregadmin/hive/types.h>
#include <rregadmin/util/path.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/diff/diff_info.h>

G_BEGIN_DECLS

/** Compare two value key cells.
 *
 * @ingroup diff_group
 */
int value_key_cell_diff(rra_path *in_path,
                        ValueKeyCell *in_kc1, ValueKeyCell *in_kc2,
                        rra_diff_info *in_di);

G_END_DECLS

#endif // RREGADMIN_DIFF_VALUE_KEY_CELL_H
