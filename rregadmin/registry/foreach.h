/*
 *
 * Authors:     James LewisMoss <jlm@racemi.com>
 *
 * Copyright 2008 Racemi Inc
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

#ifndef RREGADMIN_REGISTRY_FOREACH_H
#define RREGADMIN_REGISTRY_FOREACH_H 1

#include <rregadmin/registry/types.h>
#include <rregadmin/hive/foreach.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/util/path.h>

G_BEGIN_DECLS

/** Typedef for hive_foreach_value() use.
 *
 * @ingroup registry_registry_group
 */
typedef gboolean (*RegistryForeachHiveFunc) (
    Hive *in_hive, const rra_path *in_hive_location, gpointer in_data);

/** Run function against each key in a hive.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_foreach_hive(Registry *in_hive,
                               RegistryForeachHiveFunc in_func,
                               gpointer in_data);

/** Run function against each key in a registry.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_foreach_key(Registry *in_reg, HiveForeachKeyFunc in_func,
                              gpointer in_data);

/** Run function against each value key in a registry.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_foreach_value(Registry *in_reg,
                                HiveForeachValueFunc in_func,
                                gpointer in_data);


G_END_DECLS

#endif // RREGADMIN_REGISTRY_FOREACH_H
