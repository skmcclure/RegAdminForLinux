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

#include <rregadmin/config.h>

#include <stdio.h>
#include <string.h>

#include <glib/gi18n-lib.h>

#include <rregadmin/hive/cell.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/value_list_cell.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/hive/log.h>

/** the value list contained in a ValueKeyCell.
 *
 * \note: I suspected a value list cell has a normal cell header,
 * I was wrong.  There's no way to tell if a cell is a value list
 * without following the tree so value lists and value cells are
 * indistinguishable.
 *
 * @internal
 *
 * @ingroup vlc_hive_group
 */
struct ValueListCellData
{
    guint32 size;
    guint32 ofs[];
} RRA_VERBATIM_STRUCT;

/** Object wrapper for value list cells.
 *
 * @ingroup vlc_hive_group
 */
struct ValueListCell_
{
    Cell cell;
};

static struct ValueListCellData* get_data(ValueListCell *kc);
static Hive* get_hive(ValueListCell *kc);
static offset get_offset(ValueListCell *kc);
static gint32 value_list_cell_get_offset(ValueListCell *vlc, int no);

static inline struct ValueListCellData*
get_data(ValueListCell *kc)
{
    return (struct ValueListCellData*)cell_get_data(
        value_list_cell_to_cell(kc));
}

static Hive*
get_hive(ValueListCell *kc)
{
    return cell_get_hive(value_list_cell_to_cell(kc));
}

static offset
get_offset(ValueListCell *kc)
{
    return cell_get_offset(value_list_cell_to_cell(kc));
}

ValueListCell *
value_list_cell_alloc (Hive *hdesc, offset in_ofs, int no)
{
    return value_list_cell_from_cell(cell_alloc (hdesc, in_ofs, no * 4));
}

gboolean
value_list_cell_unalloc(ValueListCell *vlc,
                        int num_items,
                        gboolean in_recursive)
{
    rra_info(N_("Unallocating (%#010x): Items=%d, Recursive=%s"),
             offset_to_begin(cell_get_offset(value_list_cell_to_cell(vlc))),
             num_items, in_recursive ? N_("true") : N_("false"));
    if (in_recursive)
    {
        int i;
        for (i = 0; i < num_items; i++)
        {
            value_key_cell_unalloc(value_list_cell_get_entry(
                                       vlc, i, num_items));
        }
    }

    return cell_unalloc(value_list_cell_to_cell(vlc));
}

static gint32
value_list_cell_get_offset(ValueListCell *vlc, int no)
{
    return get_data(vlc)->ofs[no];
}

ValueListCell*
value_list_cell_from_cell(Cell *ptr)
{
    return (ValueListCell*)ptr;
}

Cell*
value_list_cell_to_cell(ValueListCell *vlc)
{
    return (Cell*)vlc;
}

ValueKeyCell*
value_list_cell_get_entry(ValueListCell *vlc, guint32 val_no, int num_items)
{
    if (val_no >= num_items)
    {
        return NULL;
    }

    Cell *hb = hive_get_cell_from_offset (
        get_hive(vlc), value_list_cell_get_offset(vlc, val_no));

    ValueKeyCell *ret_val = value_key_cell_from_cell (hb);

    return ret_val;
}

gboolean
value_list_cell_remove(ValueListCell *vlc, guint32 val_no, int num_items)
{
    if (val_no < num_items)
    {
        memmove (&get_data(vlc)->ofs[val_no],
                 &get_data(vlc)->ofs[val_no + 1],
                 sizeof (gint32) * (num_items - val_no));
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

gboolean
value_list_cell_add(ValueListCell *vlc, offset offset, int num_items)
{
    get_data(vlc)->ofs[num_items] = offset_to_begin(offset);
    return TRUE;
}

void
value_list_cell_debug_print (ValueListCell *vl, int no)
{
    int i;

    fprintf (stderr, "<ValueListCell offset=\"%#010x\">\n",
             offset_to_begin(get_offset(vl)));
    for (i = 0; i < no; i++)
    {
        fprintf (stderr, "  <offset value=\"%#010x\"/>\n",
                 value_list_cell_get_offset(vl, i));
    }
    fprintf (stderr, "</ValueListCell>\n");
}
