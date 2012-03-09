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

#ifndef RREGADMIN_HIVE_LX_H
#define RREGADMIN_HIVE_LX_H 1

#include <rregadmin/util/macros.h>
#include <rregadmin/util/offsets.h>
#include <rregadmin/hive/types.h>

/**
 * @defgroup klc_hive_group KeyListCell Object
 *
 * This cell contains lists of KeyCell s.  It's a very complex structure
 * taking several different forms.
 *
 * @internal
 *
 * @ingroup hive_group
 */

G_BEGIN_DECLS

/** Allocate a new key list cell.
 *
 * @ingroup klc_hive_group
 */
KeyListCell *key_list_cell_alloc (Hive *hdesc, offset p,
                                  int id, int count);

/** Copy a key list cell expanding the size to accomodate more keys.
 *
 * @ingroup klc_hive_group
 */
KeyListCell *key_list_cell_alloc_copy (Hive *hdesc, const KeyListCell *p,
                                       int count);

/** Deallocate a key list cell
 *
 * @ingroup klc_hive_group
 */
gboolean key_list_cell_unalloc(KeyListCell *klc, gboolean in_recursive);

/** Convert a Cell to a KeyListCell
 *
 * @ingroup klc_hive_group
 */
KeyListCell *key_list_cell_from_cell (Cell *p);

/** Convert a KeyListCell to a Cell.
 *
 * @ingroup klc_hive_group
 */
Cell * key_list_cell_to_cell (KeyListCell * lx);

/** Check the validity of a key list cell.
 *
 * @ingroup klc_hive_group
 */
gboolean key_list_cell_is_valid (const KeyListCell *klc);

/** Get the current amount of subkey offsets held.
 *
 * @ingroup klc_hive_group
 */
guint32 key_list_cell_get_size(const KeyListCell *klc);

/** Get the number of sublists.
 *
 * Only RI lists have sublists so 0 is returned for the rest.
 *
 * @internal
 *
 * @ingroup klc_hive_group
 */
guint32 key_list_cell_get_number_of_sublists(const KeyListCell *klc);

/** Get sublist by index.
 *
 * @internal
 *
 * @ingroup klc_hive_group
 */
KeyListCell* key_list_cell_get_sublist(KeyListCell *klc, guint32 in_entry);

/** Get the amount of subkey offsets that can be held.
 *
 * @ingroup klc_hive_group
 */
guint32 key_list_cell_get_capacity(const KeyListCell *klc);

/** Get the type of key list cell.
 *
 * @see ID_LF_KEY, ID_LI_KEY, ID_LH_KEY, ID_RI_KEY
 *
 * @ingroup klc_hive_group
 */
guint16 key_list_cell_get_type(const KeyListCell *klc);

/** Delete an entry based on index.
 *
 * @ingroup klc_hive_group
 */
gboolean key_list_cell_delete_entry (KeyListCell * lx, guint32 in_index);

/** Delete an entry based on name.
 *
 * @ingroup klc_hive_group
 */
gboolean key_list_cell_delete_entry_str (KeyListCell * lx, const char *name);

/** Get an entry based on index.
 *
 * @ingroup klc_hive_group
 */
KeyCell *key_list_cell_get_entry (const KeyListCell * lx, guint32 in_index);

/** Get an entry based on name.
 *
 * @ingroup klc_hive_group
 */
KeyCell *key_list_cell_get_entry_str (const KeyListCell * lx,
                                      const char *name);

/** Possible return values from key_list_cell_add_entry()
 *
 * This is neccessary so that the KeyCell can update its count of subkeys
 * properly.
 *
 * @ingroup klc_hive_group
 */
typedef enum {
    /** Entry was replaced.
     */
    KLC_ADD_REPLACED,
    /** Entry was added.
     */
    KLC_ADD_ADDED,
    /** Error occurred.
     */
    KLC_ADD_ERROR
} KeyListCellAddReturn;

/** Add an entry.
 *
 * @see KLC_ADD_REPLACED, KLC_ADD_ADDED, KLC_ADD_ERROR
 *
 * @ingroup klc_hive_group
 */
KeyListCellAddReturn key_list_cell_add_entry (KeyListCell * lx,
                                              KeyCell *parent,
                                              KeyCell *child);

/** Print an xml representation to stderr.
 *
 * @internal
 *
 * @ingroup klc_hive_group
 */
void key_list_cell_debug_print (KeyListCell * lxkey);

G_END_DECLS

#endif // RREGADMIN_HIVE_LX_H
