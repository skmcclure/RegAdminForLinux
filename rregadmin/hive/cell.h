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

#ifndef RREGADMIN_HIVE_CELL_H
#define RREGADMIN_HIVE_CELL_H 1

#include <rregadmin/hive/types.h>
#include <rregadmin/hive/defines.h>
#include <rregadmin/hive/hive_offset.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/util/offsets.h>

/**
 * @defgroup cell_hive_group Cell Object
 *
 * This is a generic implementation of the basic cell behavior. "Subclasses"
 * "inherit" from this "class" and implement more specific behavior.
 *
 * @ingroup hive_group
 */
G_BEGIN_DECLS

/** Possible types for cells.
 *
 * @see cell_get_type()
 *
 * @ingroup cell_hive_group
 */
typedef enum
{
    /** Type value for a KeyCell_.
     */
    CELL_TYPE_KEY = 1,
    /** Type value for a ValueKeyCell_.
     */
    CELL_TYPE_VALUE_KEY = 2,
    /** Type value for a KeyListCell_.
     */
    CELL_TYPE_KEY_LIST = 3,
    /** Type value for a ValueListCell_.
     */
    CELL_TYPE_VALUE_LIST = 4,
    /** Type value for a SecurityDescriptorCell_.
     */
    CELL_TYPE_SECURITY = 5,
    /** Type value for an unknown or undeterminable cell.
     *
     * This will be returned for ValueListCell_ s, and ValueCell_ s
     */
    CELL_TYPE_UNKNOWN = 20,
    /** Type value for an empty cell
     *
     * If the cell isn't allocated then it has no type.
     */
    CELL_TYPE_EMPTY = 21
} CellType;

/** Allocate a block of requested size if possible
 *
 * \note This function WILL CHANGE THE HIVE if successful
 *
 * @param hdesc  hive
 * @param ofs    If non NULL will try to find a cell in the bin containing
 *               the offset.
 * @param size   number of bytes to allocate
 *
 * @return NULL for failure else a Cell_
 *
 * @ingroup cell_hive_group
 */
Cell * cell_alloc (Hive *hdesc, offset ofs, int size);

/** Unallocate the cell.
 *
 * This frees the cell in the hive.
 *
 * \note This function WILL CHANGE THE HIVE if successful
 *
 * @param in_cell    The cell to unallocate
 * @return           TRUE == success
 *
 * @ingroup cell_hive_group
 */
gboolean cell_unalloc (Cell *in_cell);

/** Initialize a new cell's data.
 *
 * @ingroup cell_hive_group
 */
gboolean cell_init(Hive *hdesc, Bin *bin, offset data,
                   guint32 data_len);

/** Retrieve a Cell_ object from the offset ptr.
 *
 * @ingroup cell_hive_group
 */
Cell* cell_get(Hive *hdesc, Bin *in_bin, offset ptr);

/** Free a Cell_ object.
 *
 * \note This DOES NOT change the hive.
 *
 * @ingroup cell_hive_group
 */
void cell_delete(Cell *in_cell);

/** Get the length of data space in cell.
 *
 * @ingroup cell_hive_group
 */
guint32 cell_get_data_length (Cell *in_cell);

/** Get the raw data from the cell.
 *
 * @ingroup cell_hive_group
 */
guint8* cell_get_data(Cell *in_cell);

/** Clear all data from the cell.
 *
 * This is called when cells are freed, split, etc to make sure no old data
 * sits around.
 *
 * @ingroup cell_hive_group
 */
gboolean cell_clear (Cell *in_cell);

/** Get the hive this cell is associated with.
 *
 * @ingroup cell_hive_group
 */
Hive* cell_get_hive(Cell *in_cell);

/** Get this cell's offset.
 *
 * @ingroup cell_hive_group
 */
offset cell_get_offset(Cell *in_cell);

/** Get this cell's Bin_.
 *
 * @ingroup cell_hive_group
 */
Bin *cell_get_bin(Cell *in_cell);

/** Compare two cells for equality.
 *
 * The cells must be the same cell.  Not same data.
 *
 * @ingroup cell_hive_group
 */
gboolean cell_equal(const Cell *in_cella, const Cell *in_cellb);

/** Check the validity of a cell.
 *
 * @ingroup cell_hive_group
 */
gboolean cell_is_valid(Cell *in_cell);

/** Get the id for the cell.
 *
 * @see defines.h ID_*
 *
 * @ingroup cell_hive_group
 */
CellID cell_get_id(Cell *in_cell);

/** Get the type of the cell.
 *
 * See the defines in cell.h
 *
 * @ingroup cell_hive_group
 */
CellType cell_get_type(Cell *in_cell);

/** Get a string rep for the id.
 *
 * @ingroup cell_hive_group
 */
const char* cell_get_id_str(Cell *in_cell);

/** Get the size of cell.
 *
 * This includes the size of the header.
 *
 * @ingroup cell_hive_group
 */
guint32 cell_size (Cell *in_cell);

/** Is this cell allocated?
 *
 * @ingroup cell_hive_group
 */
gboolean cell_is_allocd (Cell *in_cell);

/** Find the cell that comes next.
 *
 * If this is the last cell in the bin returns NULL.
 *
 * @ingroup cell_hive_group
 */
Cell *cell_get_next (Cell *in_cell);

/** Find the cell that comes before.
 *
 * If this is the first cell in the bin returns NULL.
 *
 * @ingroup cell_hive_group
 */
Cell *cell_get_prev (Cell *in_cell);

/** Print an xml representation of the cell to stderr.
 *
 * @internal
 *
 * @ingroup cell_hive_group
 */
void cell_debug_print (Cell *in_cell);

/** Print a longer xml representation of the cell to stderr.
 *
 * @internal
 *
 * @ingroup cell_hive_group
 */
void cell_debug_print_long (Cell *in_cell);

/** Get an xml representation of the cell in in_output.
 *
 * @internal
 *
 * @ingroup cell_hive_group
 */
gboolean cell_get_xml_output (const Cell *in_bin, ustring *in_output,
                              gboolean in_verbose);

/** Print a cell representation to the ustring.
 *
 * @deprecated
 *
 * @ingroup cell_hive_group
 */
void cell_vdump (ustring *in_ustr, Cell *in_cell);

/** Enum for cell flags.
 *
 * @internal
 *
 * @ingroup cell_hive_group
 */
typedef enum {
    CELL_FLAG_VALID = 1 << 0,
    CELL_FLAG_SUBTYPE_VALID = 1 << 1,

    CELL_FLAG_LINKED = 1 << 16

} CellMark;

/** Set a mark.
 *
 * @internal
 *
 * @ingroup cell_hive_group
 */
void cell_set_mark (Cell *in_cell, CellMark in_mark);

/** Check a mark.
 *
 * @internal
 *
 * @ingroup cell_hive_group
 */
gboolean cell_check_mark (Cell *in_cell, CellMark in_mark);

/** Clear a mark.
 *
 * @internal
 *
 * @ingroup cell_hive_group
 */
void cell_clear_mark (Cell *in_cell, CellMark in_mark);

/** Standard header for most cell types.
 *
 * Most of the cell types use this header.
 *
 * @internal
 *
 * @ingroup cell_hive_group
 */
struct CellHeader
{
    gint32 size;
    guint16 id;
} RRA_VERBATIM_STRUCT;

typedef void (*cell_data_deleter) (void*);

/** Cell wrapper object.
 *
 * Most sub-cell types "inherit" from this type.
 *
 * @internal
 *
 * @ingroup cell_hive_group
 */
struct Cell_
{
    hive_offset ho;
    Bin *b;
    guint32 marks;
    cell_data_deleter deleter;
    Cell *next;
    Cell *prev;
    void *data;
};


G_END_DECLS

#endif // RREGADMIN_HIVE_CELL_H
