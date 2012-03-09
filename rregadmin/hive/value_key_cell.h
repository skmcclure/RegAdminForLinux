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

#ifndef RREGADMIN_HIVE_VALUE_KEY_CELL_H
#define RREGADMIN_HIVE_VALUE_KEY_CELL_H 1

#include <glib.h>

#include <rregadmin/hive/types.h>
#include <rregadmin/util/value.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/path.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/util/offsets.h>

/**
 * @defgroup vkc_hive_group ValueKeyCell Object
 *
 * Cell containing a value (possibly with ValueCells containing the
 * value's data)
 *
 * @ingroup hive_group
 */

G_BEGIN_DECLS

/** Alloc a value key cell in the hive.
 *
 * @ingroup vkc_hive_group
 */
ValueKeyCell * value_key_cell_alloc (Hive *hdesc, offset rel,
                                     const char *name, const Value *in_val);

/** Free the value key cell in the hive.
 *
 * @ingroup vkc_hive_group
 */
gboolean value_key_cell_unalloc(ValueKeyCell *vkc);

/** Convert a Cell to a ValueKeyCell doing some checks.
 *
 * @ingroup vkc_hive_group
 */
ValueKeyCell *value_key_cell_from_cell (Cell *p);

/** Convert a ValueKeyCell back to a Cell.
 *
 * @ingroup vkc_hive_group
 */
Cell* value_key_cell_to_cell(ValueKeyCell *vkc);

/** Check the ValueKeyCell's validity.
 *
 * @ingroup vkc_hive_group
 */
gboolean value_key_cell_is_valid(const ValueKeyCell *p);

/** Get the name of the value key cell.
 *
 * @ingroup vkc_hive_group
 */
ustring* value_key_cell_get_name (const ValueKeyCell *vk);

/** Get the size of the data.
 *
 * @ingroup vkc_hive_group
 */
guint32 value_key_cell_get_data_size (const ValueKeyCell *vk);

/** Get the data.
 *
 * @ingroup vkc_hive_group
 */
const guint8* value_key_cell_get_data (const ValueKeyCell *vk);

/** Get the flag data from the value key cell.
 *
 * @ingroup vkc_hive_group
 */
guint16 value_key_cell_get_flags (const ValueKeyCell *vk);

/** Get the value of the value key cell.
 *
 * @ingroup vkc_hive_group
 */
Value *value_key_cell_get_val (ValueKeyCell *vk);

/** Set the value of the value key cell.
 *
 * @ingroup vkc_hive_group
 */
gboolean value_key_cell_set_val (ValueKeyCell *in_vkc, const Value *in_val);

/** A function implementing this interface recieves a Value and a function
 * defined bit of data as parameters and returns a Value.
 *
 * The Value recieved as a parameter must either be returned or must be
 * freed.
 *
 * A NULL return indicates an error occured and modify_val will return
 * FALSE.
 *
 * @ingroup vkc_hive_group
 */
typedef Value* (*ValueKeyCellModifier) (Value *in_val, gpointer in_data);

/** Modify the value of a ValueKeyCell.
 *
 * This method takes a ValueKeyCellModifier and applies it to its value.
 * Essentially it set_vals the returned Value.
 *
 * @ingroup vkc_hive_group
 */
gboolean value_key_cell_modify_val (ValueKeyCell *in_vkc,
                                    ValueKeyCellModifier in_mod,
                                    gpointer in_mod_data);

/** Get the type of the value.
 *
 * @ingroup vkc_hive_group
 */
ValueType value_key_cell_get_type (const ValueKeyCell *vk);

/** Get the ValueCell of the ValueKeyCell.
 *
 * @internal you should normally access the value through
 *           value_key_cell_get_val()
 *
 * @ingroup vkc_hive_group
 */
ValueCell *value_key_cell_get_value_cell(const ValueKeyCell *vk);

/** Place pretty (user friendly) output for the ValueKeyCell into the
 * ustring passed in as a parameter.
 *
 * @ingroup vkc_hive_group
 */
gboolean value_key_cell_get_pretty_output (ValueKeyCell *in_vkc,
                                           const char *in_pathname,
                                           ustring *in_output);

/** Place xml output for the ValueKeyCell into the ustring passed
 * in as a parameter.
 *
 * @ingroup vkc_hive_group
 */
gboolean value_key_cell_get_xml_output (ValueKeyCell *in_vkc,
                                        ustring *in_output);

/** Output xml representation to stderr.
 *
 * @internal
 *
 * @ingroup vkc_hive_group
 */
void value_key_cell_debug_print (ValueKeyCell *key);

/** Output pretty text representation to stdout.
 *
 * @internal
 *
 * @ingroup vkc_hive_group
 */
void value_key_cell_pretty_print (ValueKeyCell *key, const char *in_pathname);

G_END_DECLS

#endif // RREGADMIN_HIVE_VALUE_KEY_CELL_H
