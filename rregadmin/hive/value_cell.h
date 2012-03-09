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

#ifndef RREGADMIN_HIVE_VALUE_CELL_H
#define RREGADMIN_HIVE_VALUE_CELL_H 1

#include <glib/gtypes.h>

#include <rregadmin/hive/types.h>
#include <rregadmin/hive/defines.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/util/offsets.h>

/**
 * @defgroup vc_hive_group ValueCell Object.
 *
 * ValueCell_ s actually contain the data for a value.  Not all values require
 * a value cell.  Some can inline the value in the ValueKeyCell_ structure.
 *
 * @internal
 *
 * @ingroup hive_group
 */

G_BEGIN_DECLS

/** Allocate a new value cell.
 *
 * @ingroup vc_hive_group
 */
ValueCell * value_cell_alloc (Hive *hdesc, offset p, const guint8 *data,
                              int data_length);
/** Deallocate a value cell.
 *
 * @ingroup vc_hive_group
 */
gboolean value_cell_unalloc(ValueCell *vc);

/** Convert a Cell_ to a ValueCell_
 *
 * @ingroup vc_hive_group
 */
ValueCell *value_cell_from_cell (Cell *p);

/** Convert a ValueCell_ to a Cell_
 *
 * @ingroup vc_hive_group
 */
Cell * value_cell_to_cell (ValueCell *nk);

/** Check the validity of the value cell.
 *
 * This basically does nothing because besides the size of the cell at
 * the begining none of the rest of the data is prescribed.
 *
 * @ingroup vc_hive_group
 */
gboolean value_cell_is_valid (const ValueCell *nk);

/** Get the length of the data in the cell.
 *
 * Not all of this data may be used for the value.  The actual size of
 * the value is contained in the ValueKeyCell_.
 *
 * @ingroup vc_hive_group
 */
guint32 value_cell_get_data_length(const ValueCell *nk);

/** Get the data in the cell.
 *
 * Not all of this data may be used for the value.  The actual size of
 * the value is contained in the ValueKeyCell_.
 *
 * @ingroup vc_hive_group
 */
guint8* value_cell_get_data(ValueCell *nk);

/** Set the data in the cell.
 *
 * Not all of this data may be used for the value.  The actual size of
 * the value is contained in the ValueKeyCell_.
 *
 * @ingroup vc_hive_group
 */
gboolean value_cell_set(ValueCell *in_vc, const guint8 *in_data,
                        guint32 in_data_len);

/** Print an xml representation to stderr.
 *
 * @internal
 *
 * @ingroup vc_hive_group
 */
void value_cell_debug_print (const ValueCell *nk);

/** Place xml output for the ValueKeyCell into the ustring passed
 * in as a parameter.
 *
 * @ingroup vkc_hive_group
 */
gboolean value_cell_get_xml_output (const ValueCell *in_vc,
                                    ustring *in_output);


G_END_DECLS

#endif // RREGADMIN_HIVE_VALUE_CELL_H
