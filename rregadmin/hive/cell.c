/*
 * Authors:     Sean Loaring
 *              Petter Nordahl-Hagen
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

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/hive/log.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/hive/bin.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/util/check.h>
#include <rregadmin/hive/hcheck_decl.h>
#include <rregadmin/hive/defines.h>
#include <rregadmin/util/malloc.h>

struct CellData
{
    struct CellHeader head;
    /* 0x0004 ??? - The data */
    guint8 data[];
} RRA_VERBATIM_STRUCT;

static gboolean cell_merge (Cell *b1, Cell *b2);
static gboolean cell_split (Cell *hbb, guint32 len);
static void cell_header_vdump (ustring *vb, Cell *hbbh);
static struct CellData* get_data(Cell *c);
static const struct CellData* get_data_const(const Cell *c);

static inline struct CellData*
get_data(Cell *c)
{
    return (struct CellData*)cell_get_data(c);
}

static inline const struct CellData*
get_data_const(const Cell *c)
{
    return (const struct CellData*)cell_get_data((Cell*)c);
}

Hive*
cell_get_hive(Cell *in_cell)
{
    return hive_offset_get_hive((hive_offset*)in_cell);
}

offset
cell_get_offset(Cell *in_cell)
{
    return hive_offset_get_offset((hive_offset*)in_cell);
}

guint8*
cell_get_data(Cell *in_cell)
{
    return offset_get_data(cell_get_offset(in_cell));
}

Bin *
cell_get_bin(Cell *in_cell)
{
    if (in_cell->b == NULL)
    {
        in_cell->b = hive_find_bin (cell_get_hive(in_cell),
                                    cell_get_offset(in_cell));
    }
    return in_cell->b;
}

Cell *
cell_alloc (Hive *hdesc, offset ofs, int size)
{
    Cell *ret_val = NULL;
    int required_size = size + sizeof (struct CellHeader);

    rra_debug("Allocating new cell of size %d", size);

    if (hive_get_state(hdesc) & HMODE_NOALLOC)
    {
        rra_warning(
            _("Hive is in no-allocation mode, can't allocate blocks."));
        return NULL;
    }

    /* Check current page first */
    if (offset_is_valid(ofs))
    {
        Bin *p = hive_find_bin (hdesc, ofs);
        ret_val = bin_find_free_cell (p, required_size);
    }

    /* Then check whole hive */
    if (!ret_val)
    {
        ret_val = hive_find_free_cell (hdesc, required_size);
    }

    if (ret_val)
    {
        cell_split (ret_val, size);
        get_data(ret_val)->head.size = -get_data(ret_val)->head.size;
        cell_clear (ret_val);
        bin_set_dirty(cell_get_bin(ret_val));
    }
    else
    {
        rra_warning(N_("failed to alloc %d bytes."), size);
    }
    return ret_val;
}

gboolean
cell_unalloc (Cell *in_cell)
{
    Cell *da_cell;
    Cell *prev_cell, *next_cell;
    Bin *p;

    rra_debug("Unallocating cell %#010x",
              offset_to_begin(cell_get_offset(in_cell)));

    //TODO: I don't get this, freeing a cell will not effect the
    //size of the file.  Isn't HMODE_NOALLOC just about not
    //growing/shrinking the file for ntfs write?
    if (hive_get_state(cell_get_hive(in_cell)) & HMODE_NOALLOC)
    {
        rra_warning(N_("Hive is in no-allocation mode, can't free blocks."));
        return FALSE;
    }

    if (!cell_is_allocd(in_cell))
    {
        return FALSE;
    }

    /* Reverse the sign of the size to make unallocated */
    get_data(in_cell)->head.size = -get_data(in_cell)->head.size;

    p = cell_get_bin(in_cell);

    prev_cell = cell_get_prev (in_cell);
    next_cell = cell_get_next (in_cell);

    da_cell = in_cell;

    /* Check cell before and after and merge with this one if either
     * is unallocated. */
    if (next_cell && !cell_is_allocd (next_cell))
    {
        cell_merge (in_cell, next_cell);
    }
    if (prev_cell && !cell_is_allocd (prev_cell))
    {
        cell_merge (prev_cell, in_cell);
        da_cell = prev_cell;
    }

    cell_clear(da_cell);

    bin_set_dirty (p);

    return TRUE;
}

gboolean
cell_init(Hive *hdesc, Bin *bin, offset data, guint32 data_len)
{
    Cell *me = cell_get(hdesc, bin, data);
    me->b = bin;
    me->next = NULL;
    me->prev = NULL;
    get_data(me)->head.size = data_len;
    get_data(me)->head.id = 0;

    if (!cell_is_valid(me))
    {
        return FALSE;
    }

    return TRUE;
}

Cell*
cell_get(Hive *hv, Bin *in_bin, offset ofs)
{
    Cell *ret_val = (Cell*)hive_retrieve_cell(hv, ofs);

    if (ret_val == NULL)
    {
        /* Cell_ hasn't been created yet so create one. */
        ret_val = rra_new_type(Cell);

        hive_offset_init((hive_offset*)ret_val, hv, ofs);
        ret_val->b = in_bin;
        ret_val->next = NULL;
        ret_val->prev = NULL;
        ret_val->marks = 0;
        ret_val->deleter = NULL;
        ret_val->data = NULL;

        hive_register_cell(hv, ret_val);
        rra_debug("Created cell %p", ret_val);
    }

    return ret_val;
}

void
cell_delete(Cell *in_cell)
{
    if (in_cell == NULL)
    {
        return;
    }

    rra_debug("Freeing cell %p", in_cell);
    offset_make_invalid(&(in_cell->ho.ofs));
    in_cell->ho.hv = (Hive*)(void*)0xefefefef;
    in_cell->b = (Bin*)(void*)0xa1a1a1a1;

    if (in_cell->data != NULL)
    {
        if (in_cell->deleter != NULL)
        {
            in_cell->deleter(in_cell->data);
        }
        else
        {
            g_free(in_cell->data);
        }
        in_cell->data = NULL;
    }

    rra_free_type(Cell, in_cell);
}

gboolean
cell_is_valid(Cell *in_cell)
{
    if (in_cell == NULL)
    {
        return FALSE;
    }
    if (cell_check_mark(in_cell, CELL_FLAG_VALID))
    {
        return TRUE;
    }

    if (!offset_is_valid(cell_get_offset(in_cell)))
        return FALSE;

    cell_set_mark(in_cell, CELL_FLAG_VALID);

    return TRUE;
}

CellType
cell_get_type(Cell *in_cell)
{
    if (!cell_is_allocd(in_cell))
    {
        return CELL_TYPE_EMPTY;
    }
    else
    {
        switch (cell_get_id(in_cell))
        {
        case ID_SK_KEY:
            return CELL_TYPE_SECURITY;
        case ID_LF_KEY:
        case ID_LH_KEY:
        case ID_LI_KEY:
        case ID_RI_KEY:
            return CELL_TYPE_KEY_LIST;
        case ID_VK_KEY:
            return CELL_TYPE_VALUE_KEY;
        case ID_NK_KEY:
            return CELL_TYPE_KEY;
        default:
            return CELL_TYPE_UNKNOWN;
        }
    }
}

void
cell_set_mark (Cell *in_cell, CellMark in_mark)
{
    in_cell->marks |= in_mark;
}

gboolean
cell_check_mark (Cell *in_cell, CellMark in_mark)
{
    return (in_cell->marks & in_mark) != 0;
}

void
cell_clear_mark (Cell *in_cell, CellMark in_mark)
{
    in_cell->marks &= ~in_mark;
}

CellID
cell_get_id(Cell *in_cell)
{
    return get_data(in_cell)->head.id;
}

const char*
cell_get_id_str(Cell *in_cell)
{
    switch (cell_get_id(in_cell))
    {
    case ID_SK_KEY:
        return "SecurityDescriptorCell";
    case ID_LF_KEY:
        return "LF_KeyListCell";
    case ID_LH_KEY:
        return "LH_KeyListCell";
    case ID_LI_KEY:
        return "LI_KeyListCell";
    case ID_RI_KEY:
        return "RI_KeyListCell";
    case ID_VK_KEY:
        return "ValueKeyCell";
    case ID_NK_KEY:
        return "KeyCell";
    default:
        return "unknown";
    }
}

gboolean
cell_is_allocd (Cell *in_cell)
{
    return (get_data(in_cell)->head.size < 0) ? TRUE : FALSE;
}

guint32
cell_size (Cell *in_cell)
{
    return labs(get_data(in_cell)->head.size);
}

guint32
cell_get_data_length (Cell *in_cell)
{
    return cell_size (in_cell) - sizeof (struct CellHeader);
}

static gboolean
cell_merge (Cell *in_cell1, Cell *in_cell2)
{
    Cell *next_cell;

    if (cell_is_allocd(in_cell1)
        || cell_is_allocd(in_cell2))
    {
        return FALSE;
    }

    rra_debug("Merging cell %#010x and %#010x",
              offset_to_begin(cell_get_offset(in_cell1)),
              offset_to_begin(cell_get_offset(in_cell2)));

    get_data(in_cell1)->head.size += get_data(in_cell2)->head.size;
    in_cell1->next = in_cell2->next;

    hive_unregister_cell(cell_get_hive(in_cell2), in_cell2);

    next_cell = cell_get_next(in_cell1);
    if (next_cell != NULL)
    {
        next_cell->prev = in_cell1;
    }

    return TRUE;
}

gboolean
cell_clear (Cell *in_cell)
{
    if (in_cell == NULL)
    {
        return FALSE;
    }

    get_data(in_cell)->head.id = 0;
    memset(get_data(in_cell)->data, 0, cell_get_data_length(in_cell));

    return TRUE;
}

gboolean
cell_equal(const Cell *in_cella, const Cell *in_cellb)
{
    return offset_equal(cell_get_offset((Cell*)in_cella),
                        cell_get_offset((Cell*)in_cellb));
}

Cell *
cell_get_prev (Cell *in_cell)
{
    Cell *result;
    Cell *next;

    if (in_cell->prev != NULL)
    {
        return in_cell->prev;
    }

    /* Since we have no last cell data we have to start at the begining
     * and look till we find ourself. */
    result = bin_first_cell (cell_get_bin(in_cell));
    next = result;

    if (cell_equal(in_cell, result))
    {
        return NULL;
    }

    while (next != NULL)
    {
        if (cell_equal(next, in_cell))
        {
            in_cell->prev = result;
            return result;
        }

        result = next;
        next = cell_get_next(result);
    }

    return NULL;
}

Cell *
cell_get_next (Cell *in_cell)
{
    if (cell_size(in_cell) == 0)
    {
        return NULL;
    }

    if (in_cell->next != NULL)
    {
        return in_cell->next;
    }

    offset ofs = offset_make_relative(cell_get_offset(in_cell),
                                      cell_size(in_cell));

    if (!offset_is_valid(ofs))
    {
        return NULL;
    }

    if (!bin_within(cell_get_bin(in_cell), ofs))
    {
        return NULL;
    }
    Cell *next = cell_get(cell_get_hive(in_cell),
                          cell_get_bin(in_cell), ofs);
    in_cell->next = next;
    return next;
}

#define HBB_MIN_LEN ( sizeof( struct CellHeader ) + 4 )
static gboolean
cell_split (Cell *in_cell, guint32 len)
{
    rra_debug("Spliting cell %#010x with length %d",
              offset_to_begin(cell_get_offset(in_cell)), len);

    if (cell_is_allocd (in_cell))
    {
        return FALSE;
    }

    if (cell_get_data_length (in_cell) < len)
    {
        return FALSE;
    }

    int new_size = len + sizeof (struct CellHeader);
    if (new_size % 8)
        new_size += 8 - (new_size % 8);

    /* If a secondary cell is large enough split in_cell and make a new
     * one */
    if (cell_size (in_cell) - new_size >= HBB_MIN_LEN)
    {
        offset new_ofs = offset_make_relative(cell_get_offset(in_cell),
                                              new_size);
        if (offset_is_valid(new_ofs))
        {
            cell_init(cell_get_hive(in_cell), cell_get_bin(in_cell), new_ofs,
                      cell_size(in_cell) - new_size);
            get_data(in_cell)->head.size = new_size;
            in_cell->next = NULL;
        }
    }
    return TRUE;
}

gboolean
cell_get_xml_output (const Cell *in_cell, ustring *in_output,
                     gboolean in_verbose)
{
    if (in_verbose)
    {
        ustr_printfa (in_output, "<Cell offset=\"%#010x\">\n",
                      offset_to_begin (cell_get_offset((Cell*)in_cell)));
        ustr_printfa (in_output, "  <allocated value =\"%s\"/>\n",
                      cell_is_allocd ((Cell*)in_cell) ? "true" : "false");
        ustr_printfa (in_output, "  <size value=\"%d\"/>\n",
                      cell_size ((Cell*)in_cell));
        ustr_printfa (in_output, "  <type value=\"%#06x\" str=\"%s\"/>\n",
                      cell_get_id((Cell*)in_cell),
                      cell_get_id_str((Cell*)in_cell));
        ustr_printfa (in_output, "  <data>\n");
        ustr_hexdumpa(in_output, (const guint8*)get_data_const(in_cell), 0,
                      cell_get_data_length((Cell*)in_cell), TRUE, FALSE);
        ustr_printfa (in_output, "\n</data>\n");
        ustr_printfa (in_output, "</Cell>\n");
    }
    else
    {
        ustr_printfa (in_output, "<Cell offset=\"%#010x\"",
                      offset_to_begin (cell_get_offset((Cell*)in_cell)));
        // ustr_printfa (in_output, ", ptr=\"%p\"", p);
        ustr_printfa (in_output, ", alloced=\"%s\"",
                      cell_is_allocd ((Cell*)in_cell) ? "Y" : "N");
        ustr_printfa (in_output, ", size=\"%d\"",
                      cell_size ((Cell*)in_cell));
        ustr_printfa (in_output, ", type_id=\"%#06x\"",
                      get_data_const(in_cell)->head.id);
        ustr_printfa (in_output, ", type=\"%s\"/>\n",
                      cell_get_id_str((Cell*)in_cell));
    }
    return TRUE;
}

void
cell_debug_print (Cell *in_cell)
{
    ustring *xml_out = ustr_new();
    cell_get_xml_output(in_cell, xml_out, FALSE);
    fprintf (stderr, "%s", ustr_as_utf8(xml_out));
    ustr_free(xml_out);
}

void
cell_debug_print_long (Cell *in_cell)
{
    ustring *xml_out = ustr_new();
    cell_get_xml_output(in_cell, xml_out, TRUE);
    fprintf (stderr, "%s", ustr_as_utf8(xml_out));
    ustr_free(xml_out);
}

static void
cell_header_vdump (ustring *in_ustr, Cell *in_cell)
{
    ustr_printf (in_ustr, "== Bin Block (%08x)\n",
                 offset_to_begin (cell_get_offset(in_cell)));
    ustr_printfa (in_ustr, "Allocated:              %d\n",
                  cell_is_allocd (in_cell));
    ustr_printfa (in_ustr, "Size:                   (%d/%#x)\n",
                  cell_size (in_cell), cell_size (in_cell));
}

void
cell_vdump (ustring *in_ustr, Cell *in_cell)
{
    cell_header_vdump (in_ustr, in_cell);

    ustr_printfa (in_ustr, "Contains:               %s\n",
                  cell_get_id_str(in_cell));
    ustr_hexdump (in_ustr, (guint8*)get_data(in_cell)->data,
                  0, cell_get_data_length (in_cell), 1, FALSE);
}

gboolean
hcheck_cell_pass(Hive *in_hive, RRACheckData *in_data)
{
    if (in_hive == NULL)
    {
        return FALSE;
    }

    gboolean ret_val = TRUE;

    Bin *b;
    for (b = hive_get_first_bin(in_hive);
         b != NULL;
         b = bin_get_next_bin(b))
    {
        offset end_of_bin = offset_make_relative(bin_get_offset(b),
                                                 bin_size(b) - 1);
        if (offset_to_begin (end_of_bin) == 0)
        {
            rra_check_warning(
                in_data,
                _("Bad end offset for end of bin from: %d and %d"),
                bin_get_offset(b), bin_size(b));
            bin_debug_print(b, TRUE);
        }

        Cell *last_c = NULL;
        Cell *c;
        for (c = bin_first_cell(b);
             c != NULL;
             last_c = c, c = cell_get_next(c))
        {
            rra_check_checking(in_data, _("valid size"));
            if (cell_size(c) < sizeof(guint32) * 2)
            {
                rra_check_error(in_data,
                                _("invalid cell size: %d"),
                                cell_size(c));
                ret_val = FALSE;
            }

            rra_check_checking(in_data, _("last cell correct"));
            if (cell_get_prev(c) != last_c)
            {
                rra_check_error(in_data,
                                _("last cell incorrect"));
                ret_val = FALSE;
            }

            rra_check_checking(in_data, _("cell stays within bin"));
            offset end_of_cell = offset_make_relative(cell_get_offset(c),
                                                      cell_size(c) - 1);
            int cmp_val = offset_compare(end_of_bin, end_of_cell);
            if (cmp_val < 0)
            {
                rra_check_error(
                    in_data,
                    _("cell extends past end of bin: "
                      "Bin(%d) vs Cell(%d) => %d"),
                    offset_to_begin(end_of_bin),
                    offset_to_begin(end_of_cell),
                    cmp_val);
                ret_val = FALSE;
            }
        }
    }

    return TRUE;
}
