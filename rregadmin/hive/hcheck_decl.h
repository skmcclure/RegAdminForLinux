/*
 * Authors:     James LewisMoss <jlm@racemi.com>
 *
 * Copyright 2006,2008 Racemi Inc
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


#ifndef RREGADMIN_HIVE_HCHECK_DECL_H
#define RREGADMIN_HIVE_HCHECK_DECL_H 1

#include <rregadmin/util/macros.h>
#include <rregadmin/hive/hive_check.h>

G_BEGIN_DECLS

/** Functions that will be used in passes of the check must look like this.
 *
 * @ingroup hive_group
 */
typedef gboolean (*HC_Pass) (Hive *in_hive, RRACheckData *in_data);

/** Do the hive pass of the hive_check function.
 *
 * @ingroup hive_group
 */
gboolean hcheck_hive_pass(Hive *in_hive, RRACheckData *in_data);

/** Do the bin pass of the hive_check function.
 *
 * @ingroup hive_group
 */
gboolean hcheck_bin_pass(Hive *in_hive, RRACheckData *in_data);

/** Do the cell pass of the hive_check function.
 *
 * @ingroup hive_group
 */
gboolean hcheck_cell_pass(Hive *in_hive, RRACheckData *in_data);

/** Do the key cell pass of the hive_check function.
 *
 * @ingroup hive_group
 */
gboolean hcheck_key_cell_pass(Hive *in_hive, RRACheckData *in_data);

/** Do the value key cell pass of the hive_check function.
 *
 * @ingroup hive_group
 */
gboolean hcheck_value_key_cell_pass(Hive *in_hive, RRACheckData *in_data);

/** Do the key list cell pass of the hive_check function.
 *
 * @ingroup hive_group
 */
gboolean hcheck_key_list_cell_pass(Hive *in_hive, RRACheckData *in_data);

/** Do the security descriptor cell pass of the hive_check function.
 *
 * @ingroup hive_group
 */
gboolean hcheck_security_descriptor_cell_pass(Hive *in_hive,
                                              RRACheckData *in_data);

/** Do the garbage collection pass of the hive_check function.
 *
 * @ingroup hive_group
 */
gboolean hcheck_gc_pass(Hive *in_hive, RRACheckData *in_data);

G_END_DECLS

#endif // RREGADMIN_HIVE_HCHECK_DECL_H
