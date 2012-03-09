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

#ifndef RREGADMIN_DIFF_HIVE_H
#define RREGADMIN_DIFF_HIVE_H 1

#include <glib.h>

#include <rregadmin/hive/types.h>
#include <rregadmin/util/path.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/diff/diff_info.h>

G_BEGIN_DECLS

/** Get a diff between two hives.
 *
 * @ingroup diff_group
 */
int hive_diff(rra_path *in_path, Hive *in_from_hive, Hive *in_to_hive,
              rra_diff_info *in_di);

/** Compare two hives.
 *
 * If in_verbose is true print out the differences.
 *
 * Comparison is done in the following order:
 *
 *  1. Hives with larger versions are greater.
 *  2. Compare hive names.
 *  3. Compare root keys with key_cell_compare.
 *
 * @ingroup diff_group
 */
int hive_compare(rra_path *in_path, Hive *in_hive1, Hive *in_hive2);

/** Compare two hive versions.
 *
 * @ingroup diff_group
 */
int hive_version_compare(const HiveVersion *in_v1, const HiveVersion *in_v2);

/** Check to see if two hives are the same.
 *
 * If in_verbose is true print out the differences.
 *
 * @ingroup diff_group
 */
gboolean hive_equal(rra_path *in_path, Hive *in_hive1, Hive *in_hive2);


G_END_DECLS

#endif // RREGADMIN_DIFF_HIVE_H
