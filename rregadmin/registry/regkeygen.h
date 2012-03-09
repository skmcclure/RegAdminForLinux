/*
 *
 * Authors:       James LewisMoss <jlm@racemi.com>
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

#ifndef RREGADMIN_REGISTRY_REGKEYGEN_H
#define RREGADMIN_REGISTRY_REGKEYGEN_H 1

#include <glib/garray.h>

#include <rregadmin/registry/registry.h>
#include <rregadmin/registry/hive_info.h>

G_BEGIN_DECLS

/** Generate a list of RegistryKey s for the hive loaded.
 *
 * @ingroup registry_group
 */
GPtrArray* reg_key_gen(Registry* in_reg, hive_id in_id);

G_END_DECLS

#endif // RREGADMIN_REGISTRY_REGKEYGEN_H
