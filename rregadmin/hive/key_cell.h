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

#ifndef RREGADMIN_HIVE_KEY_CELL_H
#define RREGADMIN_HIVE_KEY_CELL_H 1

#include <glib.h>

#include <rregadmin/hive/types.h>
#include <rregadmin/hive/defines.h>
#include <rregadmin/util/value.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/path.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/util/offsets.h>
#include <rregadmin/secdesc/security_descriptor.h>

/**
 * @defgroup kc_hive_group KeyCell Object
 *
 * This object handles modifying, creating, etc KeyCells.
 *
 * @ingroup hive_group
 */

G_BEGIN_DECLS

/** Type of key cell type
 *
 * @ingroup kc_hive_group
 */
typedef guint16 key_cell_type;

/** Enum of possible key cell types.
 *
 * @ingroup kc_hive_group
 */
typedef enum
{
    /** Indicator that this is a root KeyCell.
     */
    KEY_ROOT = 0x2c,

    /** Indicator that this is a regular KeyCell.
     */
    KEY_NORMAL = 0x20,

    /** Indicator that this is a link KeyCell.
     *
     * @note I've never seen one of these.
     */
    KEY_LINK = 0x10,

} KeyCellType;

/** Allocate a new KeyCell in the hive.
 *
 * @ingroup kc_hive_group
 */
KeyCell * key_cell_alloc (Hive *in_hive, offset in_ofs,
                          const ustring *in_name);

/** Deallocate a KeyCell.
 *
 * @ingroup kc_hive_group
 */
gboolean key_cell_unalloc(KeyCell *in_kc, gboolean in_recursive);

/** Allocate the root key cell.
 *
 * @internal This is used by hive_new() but should almost never be used
 *           by anything else.
 *
 * @ingroup kc_hive_group
 */
KeyCell * key_cell_alloc_root (Hive *in_hive);

/** Convert a Cell to a KeyCell.
 *
 * @ingroup kc_hive_group
 */
KeyCell *key_cell_from_cell (Cell *in_cell);

/** Convert a KeyCell to a Cell.
 *
 * @ingroup kc_hive_group
 */
Cell * key_cell_to_cell (KeyCell *in_kc);

/** Check the validity of a KeyCell.
 *
 * @ingroup kc_hive_group
 */
gboolean key_cell_is_valid (KeyCell *in_kc);

/** Get the parent KeyCell of this KeyCell.
 *
 * @ingroup kc_hive_group
 */
KeyCell *key_cell_get_parent (KeyCell *in_kc);

/** Get a string representation of the type of this KeyCell.
 *
 * @see key_cell_get_type_id()
 *
 * @ingroup kc_hive_group
 */
const char *key_cell_get_type_str(KeyCell *in_kc);

/** Get the type of this KeyCell.
 *
 * @see KEY_ROOT, KEY_NORMAL, KEY_LINK
 *
 * @ingroup kc_hive_group
 */
key_cell_type key_cell_get_type_id(KeyCell *in_kc);

/** Get the name of the KeyCell.
 *
 * @note You must ustr_free() the return value.
 *
 * @ingroup kc_hive_group
 */
ustring* key_cell_get_name (KeyCell *in_kc);

/** Get the classname of the KeyCell.
 *
 * @note You must ustr_free() the return value.
 *
 * @ingroup kc_hive_group
 */
ustring* key_cell_get_classname (KeyCell *in_kc);

/** Compare two key cell's names.
 *
 * @ingroup kc_hive_group
 */
int key_cell_compare_names(KeyCell *in_kc1, KeyCell *in_kc2);

/** Get the number of values.
 *
 * @ingroup kc_hive_group
 */
guint32 key_cell_get_number_of_values(KeyCell *in_kc);

/** Get value by index.
 *
 * @ingroup kc_hive_group
 */
ValueKeyCell *key_cell_get_value (KeyCell *in_kc, guint32 in_index);

/** Get value by name.
 *
 * @ingroup kc_hive_group
 */
ValueKeyCell *key_cell_get_value_str (KeyCell *in_kc, const char *in_name);

/** Get value by ustring name.
 *
 * @ingroup kc_hive_group
 */
ValueKeyCell *key_cell_get_value_ustr (KeyCell *in_kc, const ustring *in_name);

/** Delete value by index.
 *
 * @ingroup kc_hive_group
 */
gboolean key_cell_delete_value (KeyCell *in_kc, guint32 in_index);

/** Delete value by name.
 *
 * @ingroup kc_hive_group
 */
gboolean key_cell_delete_value_str (KeyCell *in_kc, const char *in_name);

/** Delete value by ustring name.
 *
 * @ingroup kc_hive_group
 */
gboolean key_cell_delete_value_ustr (KeyCell *in_kc, const ustring *in_name);

/** Add a value.
 *
 * @ingroup kc_hive_group
 */
ValueKeyCell* key_cell_add_value (KeyCell *in_kc, const char *in_name,
                                  const Value *in_val);

/** Add a value.
 *
 * @ingroup kc_hive_group
 */
ValueKeyCell* key_cell_add_value_ustr (KeyCell *in_kc, const ustring *in_name,
                                       const Value *in_val);

/** Get the number of subkeys.
 *
 * @ingroup kc_hive_group
 */
guint32 key_cell_get_number_of_subkeys (KeyCell *in_kc);

/** Get a subkey by index.
 *
 * @ingroup kc_hive_group
 */
KeyCell *key_cell_get_subkey (KeyCell *in_kc, guint32 in_index);

/** Get a subkey by name.
 *
 * @ingroup kc_hive_group
 */
KeyCell *key_cell_get_subkey_str (KeyCell *in_kc, const char *in_name);

/** Get a subkey by ustring name.
 *
 * @ingroup kc_hive_group
 */
KeyCell *key_cell_get_subkey_ustr (KeyCell *in_kc, const ustring *in_name);

/** Delete a subkey by index.
 *
 * @ingroup kc_hive_group
 */
gboolean key_cell_delete_subkey (KeyCell *in_kc, guint32 in_index);

/** Delete a subkey by name.
 *
 * @ingroup kc_hive_group
 */
gboolean key_cell_delete_subkey_str (KeyCell *in_kc, const char *in_name);

/** Delete a subkey by ustring name.
 *
 * @ingroup kc_hive_group
 */
gboolean key_cell_delete_subkey_ustr (KeyCell *in_kc, const ustring *in_name);

/** Add a subkey.
 *
 * @ingroup kc_hive_group
 */
KeyCell *key_cell_add_subkey (KeyCell *in_kc, const ustring *in_name);

/** Set the KeyCell's KeyListCell.
 *
 * @internal
 *
 * @ingroup kc_hive_group
 */
gboolean key_cell_set_key_list_cell(KeyCell *in_kc, KeyListCell *in_klc,
                                    gboolean in_recursive);

/** Get the KeyCell's KeyListCell.
 *
 * @internal
 *
 * @ingroup kc_hive_group
 */
KeyListCell *key_cell_get_key_list_cell (KeyCell *in_kc);

/** Get the KeyCell's ValueListCell.
 *
 * @internal
 *
 * @ingroup kc_hive_group
 */
ValueListCell *key_cell_get_value_list_cell (KeyCell *in_kc);

/** Get the KeyCell's SecurityDescriptorCell.
 *
 * @internal
 *
 * @ingroup kc_hive_group
 */
SecurityDescriptorCell* key_cell_get_security_descriptor_cell (KeyCell *in_kc);

/** Get the KeyCell's SecurityDescriptor.
 *
 * @ingroup kc_hive_group
 */
const SecurityDescriptor* key_cell_get_secdesc (KeyCell *in_kc);

/** Set the KeyCell's SecurityDescriptor.
 *
 * @ingroup kc_hive_group
 */
gboolean key_cell_set_secdesc (KeyCell *in_kc,
                               const SecurityDescriptor *in_secdesc);

/** Get the KeyCell's classname ValueCell.
 *
 * @internal
 *
 * @ingroup kc_hive_group
 */
ValueCell* key_cell_get_classname_value_cell (KeyCell *in_kc);

/** Print xml representation to stderr.
 *
 * @internal
 *
 * @ingroup kc_hive_group
 */
void key_cell_debug_print (KeyCell *in_kc);

/** Print a pretty text representation to stdout.
 *
 * @internal
 *
 * @ingroup kc_hive_group
 */
void key_cell_pretty_print (KeyCell *in_kc, const char *in_pathname);

/** Generate pretty output into in_output.
 *
 * @ingroup kc_hive_group
 */
gboolean key_cell_get_pretty_output (KeyCell *in_kc, const char *in_pathname,
                                     ustring *in_output);

/** Generate xml output into in_output.
 *
 * @ingroup kc_hive_group
 */
gboolean key_cell_get_xml_output (KeyCell *in_kc, ustring *in_output);

/** Generate parseable output into in_output.
 *
 * @ingroup kc_hive_group
 */
gboolean key_cell_get_parseable_output (KeyCell *in_kc,
                                        const char *in_pathname,
                                        ustring *in_output);

G_END_DECLS

#endif // RREGADMIN_HIVE_KEY_CELL_H
