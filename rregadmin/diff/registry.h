/*
 * Authors:     James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi Inc
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

#ifndef RREGADMIN_DIFF_REGISTRY_H
#define RREGADMIN_DIFF_REGISTRY_H 1

#include <glib.h>

#include <rregadmin/registry/types.h>
#include <rregadmin/diff/diff_info.h>

G_BEGIN_DECLS

/** Get diff information between two registries.
 *
 * @ingroup diff_group
 */
rra_diff_info* registry_diff(const Registry *in_reg1, const Registry *in_reg2);

/** Compare two registries.
 *
 * @ingroup diff_group
 */
int registry_compare(const Registry *in_reg1, const Registry *in_reg2);

/** Check if two registries are equal.
 *
 * @ingroup diff_group
 */
gboolean registry_equal(const Registry *in_reg1, const Registry *in_reg2);

G_END_DECLS

#endif // RREGADMIN_DIFF_REGISTRY_H
