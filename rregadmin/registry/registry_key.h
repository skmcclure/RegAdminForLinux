/*
 *
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

#ifndef RREGADMIN_REGISTRY_REGISTRY_KEY_H
#define RREGADMIN_REGISTRY_REGISTRY_KEY_H 1

#include <rregadmin/registry/registry.h>
#include <rregadmin/hive/types.h>
#include <rregadmin/util/path.h>
#include <rregadmin/util/macros.h>

/**
 * @defgroup registry_key_registry_group RegistryKey Object
 *
 * This object is a key into a specific part of the registry.
 *
 * @ingroup registry_group
 */

G_BEGIN_DECLS

/** Enum for registry key types
 *
 * @ingroup registry_key_registry_group
 */
typedef enum
{
    /** Meta RegistryKey type id.
     *
     * @ingroup registry_key_registry_group
     */
    REG_KEY_TYPE_META = 1,

    /** Base RegistryKey type id.
     *
     * @ingroup registry_key_registry_group
     */
    REG_KEY_TYPE_BASE = 2,

    /** Link RegistryKey type id.
     *
     * @ingroup registry_key_registry_group
     */
    REG_KEY_TYPE_LINK = 3,

} RegistryKeyType;

/** Get the first registry key id.
 *
 * @ingroup registry_key_registry_group
 */
reg_id registry_key_id_start(void);

/** Create a new base key.
 *
 * A base key has a hive associated with it and points to the top of the
 * hive tree.
 *
 * @ingroup registry_key_registry_group
 */
RegistryKey* registry_key_new_base(Registry *in_reg,
                                   hive_id in_id,
                                   const char *in_absolute_path);

/** Create a new link key.
 *
 * A link key has a hive and KeyCell associated with it and points to
 * a location further into the hive's tree.
 *
 * @ingroup registry_key_registry_group
 */
RegistryKey* registry_key_new_link(Registry *in_reg,
                                   hive_id in_id,
                                   const char *in_absolute_path,
                                   KeyCell *in_kc);

/** Create a new meta key.
 *
 * A meta key doesn't have any actual data associated with it.  Instead
 * it's a placeholder for a possible location of a hive.  This allows
 * users to refer to a particular hive statically.
 *
 * @ingroup registry_key_registry_group
 */
RegistryKey* registry_key_new_meta(Registry *in_reg,
                                   const char *in_absolute_path,
                                   reg_id in_id);

/** Free the registry key.
 *
 * Only copies of registry keys can be freed with this method.  This avoids
 * users of the library freeing a key linked into the Registry.
 *
 * @ingroup registry_key_registry_group
 */
gboolean registry_key_free (const RegistryKey *in_key);

/** Free a registry key.
 *
 * This will free any registry key.
 *
 * @internal
 *
 * @ingroup registry_key_registry_group
 */
gboolean registry_key_free_force (RegistryKey *in_key);

/** Copy a registry key
 *
 * @ingroup registry_key_registry_group
 */
RegistryKey* registry_key_copy(const RegistryKey *in_key);

/** Get the path associated with this key.
 *
 * @ingroup registry_key_registry_group
 */
const rra_path* registry_key_get_path(const RegistryKey *in_key);

/** Get the type of this key.
 *
 * @ingroup registry_key_registry_group
 */
RegistryKeyType registry_key_get_type(const RegistryKey *in_key);

/** Get this key's id.
 *
 * @ingroup registry_key_registry_group
 */
reg_id registry_key_get_id(const RegistryKey *in_key);

/** Get the key cell associated with this key.
 *
 * @ingroup registry_key_registry_group
 */
KeyCell* registry_key_get_key_cell(const RegistryKey *in_key);

/** Set the key cell associated with this key.
 *
 * \note You may only do this on copies of registry keys.
 *
 * @ingroup registry_key_registry_group
 */
gboolean registry_key_set_key_cell(RegistryKey *in_key, KeyCell *in_cell);

/** Get the hive associated with this key.
 *
 * @ingroup registry_key_registry_group
 */
Hive* registry_key_get_hive(const RegistryKey *in_key);

/** Get the type of hive associated with this key.
 *
 * @ingroup registry_key_registry_group
 */
int registry_key_get_hive_type(const RegistryKey *in_key);

/** Get the hive's filename.
 *
 * @ingroup registry_key_registry_group
 */
const char* registry_key_get_hive_filename(const RegistryKey *in_key);

/** Get the hive's id.
 *
 * @ingroup registry_key_registry_group
 */
hive_id registry_key_get_hive_id(const RegistryKey *in_key);

G_END_DECLS

#endif // RREGADMIN_REGISTRY_REGISTRY_KEY_H
