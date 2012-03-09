/*
 * Authors:     James LewisMoss <jlm@racemi.com>
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

#ifndef RREGADMIN_REGISTRY_REGISTRY_H
#define RREGADMIN_REGISTRY_REGISTRY_H 1

#include <glib.h>

#include <rregadmin/hive/types.h>
#include <rregadmin/util/value.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/path.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/registry/types.h>
#include <rregadmin/hive/foreach.h>
#include <rregadmin/registry/hive_info.h>

/**
 * @defgroup registry_registry_group Registry Object
 *
 * This object abstracts out access to a group of hives so that the whole
 * looks like a windows registry.
 *
 * @ingroup registry_group
 */

G_BEGIN_DECLS

/** ID for the root key.
 *
 * @ingroup registry_registry_group
 */
extern reg_id ID_HKEY_CLASSES_ROOT;

/** ID for the current user key.
 *
 * @ingroup registry_registry_group
 */
extern reg_id ID_HKEY_CURRENT_USER;

/** ID for the local machine key.
 *
 * @ingroup registry_registry_group
 */
extern reg_id ID_HKEY_LOCAL_MACHINE;

/** ID for the users key.
 *
 * @ingroup registry_registry_group
 */
extern reg_id ID_HKEY_USERS;

/** ID for the performance data key.
 *
 * @ingroup registry_registry_group
 */
extern reg_id ID_HKEY_PERFORMANCE_DATA;

/** ID for the current configuration key.
 *
 * @ingroup registry_registry_group
 */
extern reg_id ID_HKEY_CURRENT_CONFIG;

/** ID for the dynamic data key.
 *
 * @ingroup registry_registry_group
 */
extern reg_id ID_HKEY_DYN_DATA;


/** Create a new registry.
 *
 * @ingroup registry_registry_group
 */
Registry* registry_new(void) G_GNUC_WARN_UNUSED_RESULT;

/** Create a registry and load the standard hives from the windows
 * directory.
 *
 * @see registry_load_win_dir
 *
 * @ingroup registry_registry_group
 */
Registry *registry_new_win_dir(const char *in_dir) G_GNUC_WARN_UNUSED_RESULT;

/** Delete a registry.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_free(Registry *in_reg);

/** Load the standard hives from the windows directory.
 *
 * This group of hives is:
 * - system32/config/systemprofile/ntuser.dat
 * - system32/config/default
 * - system32/config/SAM
 * - system32/config/SECURITY
 * - system32/config/software
 * - system32/config/system
 * - system32/config/userdiff
 * Some missing hives are ignored.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_load_win_dir(Registry *in_reg, const char *in_dir);

/** Load a hive into the registry.
 *
 * It will be mounted into the tree at a hive type specific location.
 *
 * @ingroup registry_registry_group
 */
hive_id registry_load_hive(Registry *in_reg, const char *in_filename);

/** Load a hive into the registry specifying a mount location.
 *
 * @ingroup registry_registry_group
 */
hive_id registry_load_hive_full(Registry *in_reg, const char *in_filename,
                                const char *in_mount_point);

/** Unload a hive by hive_id.
 *
 * \note This fails if the hive is modified.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_unload(Registry *in_reg, hive_id in_id);

/** Unload all hives.
 *
 * \note This fails if any hives are modified.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_unload_all(Registry *in_reg);

/** Unload all hives even if they are modified.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_unload_all_force(Registry *in_reg);

/** Write out any modified hives.
 *
 * Only hives that have been modified will be written out.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_flush_all(Registry *in_reg);

/** Write out hive if it has been modified.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_flush(Registry *in_reg, hive_id in_id);

/** Registry options.
 *
 * @ingroup registry_registry_group
 */
typedef enum
{
    /** Backup hives if they are going to be written over.
     */
    REG_OPT_BACKUP = 1 << 0,
    REG_OPT_CHECK_HIVES_READ = 1 << 1,
    REG_OPT_CHECK_HIVES_WRITE = 1 << 2,
    REG_OPT_CHECK_HIVES = REG_OPT_CHECK_HIVES_READ | REG_OPT_CHECK_HIVES_WRITE
} RegistryOption;

/** Set an option.
 *
 * @see RegistryOption
 *
 * @ingroup registry_registry_group
 */
gboolean registry_set_option(Registry *in_reg, RegistryOption in_opt);

/** Unset an option.
 *
 * @see RegistryOption
 *
 * @ingroup registry_registry_group
 */
gboolean registry_unset_option(Registry *in_reg, RegistryOption in_opt);

/** Check if an option is set.
 *
 * @see RegistryOption
 *
 * @ingroup registry_registry_group
 */
gboolean registry_has_option(const Registry *in_reg, RegistryOption in_opt);

/** Get a RegistryKey from the registry key id.
 *
 * @ingroup registry_registry_group
 */
const RegistryKey *registry_get_registry_key(const Registry *in_reg,
                                             reg_id in_id);

/** Search for the registry key based on the path given.
 *
 * @ingroup registry_registry_group
 */
const RegistryKey *registry_find_registry_key(const Registry *in_reg,
                                              const char *in_path);

/** Is the path a key?
 *
 * @ingroup registry_registry_group
 */
gboolean registry_path_is_key(const Registry *in_reg, const char *in_path);

/** Get the KeyCell associated with the path.
 *
 * @ingroup registry_registry_group
 */
KeyCell* registry_path_get_key(const Registry *in_reg, const char *in_path);

/** Add a key.
 *
 * @ingroup registry_registry_group
 */
KeyCell* registry_path_add_key(const Registry *in_reg, const char *in_path);

/** Delete a key.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_path_delete_key(const Registry *in_reg, const char *in_path);

/** Check to see if a key can be deleted.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_path_is_deletable_key(const Registry *in_reg,
                                        const char *in_path);

/** Is the value name a value at path?
 *
 * @ingroup registry_registry_group
 */
gboolean registry_path_is_value(const Registry *in_reg,
                                const char *in_path,
                                const char *in_value_name);

/** Get ValueKeyCell associated with the value name at path.
 *
 * @ingroup registry_registry_group
 */
ValueKeyCell* registry_path_get_value(const Registry *in_reg,
                                      const char *in_path,
                                      const char *in_value_name);

/** Add a value.
 *
 * @ingroup registry_registry_group
 */
ValueKeyCell* registry_path_add_value(const Registry *in_reg,
                                      const char *in_path,
                                      const char *in_value_name,
                                      const Value *in_val);

/** Delete a value.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_path_delete_value(const Registry *in_reg,
                                    const char *in_path,
                                    const char *in_value_name);

/** Get the number of hives loaded.
 *
 * @ingroup registry_registry_group
 */
gint registry_number_of_hives(const Registry *in_reg);

/** Has anything in the registry been modified?
 *
 * @ingroup registry_registry_group
 */
gboolean registry_is_dirty(const Registry *in_reg);

/** Set the registry as modified.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_set_dirty(Registry *in_reg, gboolean in_value);

/** Get the hive_info struct from the hive_id.
 *
 * @ingroup registry_registry_group
 */
const hive_info* registry_get_hive_info(const Registry *in_reg, hive_id in_id);

/** Check to see if the registry contains the specified HiveType.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_contains_hivetype(const Registry *in_reg,
                                    HiveType in_type);

/** Get the Hive from the hive_id.
 *
 * @ingroup registry_registry_group
 */
Hive* registry_id_get_hive(const Registry *in_reg, hive_id in_id);

/** Get the type of hive associated with the hive_id.
 *
 * @ingroup registry_registry_group
 */
int registry_id_get_hive_type(const Registry *in_reg, hive_id in_id);

/** Get the filename of the hive associated with the hive_id.
 *
 * @ingroup registry_registry_group
 */
const char* registry_id_get_hive_filename(const Registry *in_reg,
                                          hive_id in_id);

/** Get a debug string describing the registry.
 *
 * @ingroup registry_registry_group
 */
gboolean registry_get_xml_output(const Registry *in_reg, ustring *in_output);

/** Get a base registry key associated with the hive_id.
 *
 * @internal
 *
 * @ingroup registry_registry_group
 */
const RegistryKey *registry_get_base_registry_key(const Registry *in_reg,
                                                  hive_id in_id);


G_END_DECLS

#endif // RREGADMIN_REGISTRY_REGISTRY_H
