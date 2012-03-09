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

#ifndef RREGADMIN_HIVE_VALUE_LIST_CELL_H
#define RREGADMIN_HIVE_VALUE_LIST_CELL_H 1

#include <rregadmin/hive/types.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/util/offsets.h>

/**
 * @defgroup vlc_hive_group ValueListCell Object
 *
 * Object that contains a list of values.
 *
 * @internal
 *
 * @ingroup hive_group
 */

G_BEGIN_DECLS

/** Allocate a new value list cell.
 *
 * @ingroup vlc_hive_group
 */
ValueListCell* value_list_cell_alloc (Hive *hdesc, offset nk, int no);

/** Deallocate a value list cell.
 *
 * @ingroup vlc_hive_group
 */
gboolean value_list_cell_unalloc(ValueListCell *vlc, int num_items,
                                 gboolean in_recursive);

/** Convert a Cell to a ValueListCell.
 *
 * @ingroup vlc_hive_group
 */
ValueListCell* value_list_cell_from_cell(Cell *ptr);

/** Convert a ValueListCell to a Cell.
 *
 * @ingroup vlc_hive_group
 */
Cell* value_list_cell_to_cell(ValueListCell *vlc);

/** Get an item from the value list cell.
 *
 * @ingroup vlc_hive_group
 */
ValueKeyCell* value_list_cell_get_entry(ValueListCell *vlc, guint32 val_no,
                                        int num_items);

/** Remove an item from the value list cell.
 *
 * @ingroup vlc_hive_group
 */
gboolean value_list_cell_remove(ValueListCell *vlc, guint32 val_no,
                                int num_items);

/** Add an item to the value list cell.
 *
 * @ingroup vlc_hive_group
 */
gboolean value_list_cell_add(ValueListCell *vlc, offset in_ofs,
                             int num_items);

/** Print an xml representation of the value list cell.
 *
 * @internal
 *
 * @ingroup vlc_hive_group
 */
void value_list_cell_debug_print (ValueListCell *vl, int no);

G_END_DECLS

#endif // RREGADMIN_HIVE_VALUE_LIST_CELL_H
