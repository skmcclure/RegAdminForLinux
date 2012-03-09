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

#include <string.h>
#include <stdio.h>

#include <glib/gi18n-lib.h>

#include <rregadmin/hive/value_cell.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/log.h>

/** Data description of value cell.
 *
 * @internal
 *
 * @ingroup vc_hive_group
 */
struct ValueCellData
{
    gint32 size;
    guint8 data[];
} RRA_VERBATIM_STRUCT;

/** Object wrapper for value cell.
 *
 * @ingroup vc_hive_group
 */
struct ValueCell_
{
    Cell cell;
};

static struct ValueCellData* get_data(ValueCell *kc);
/* static Hive* get_hive(ValueCell *kc); */
static offset get_offset(ValueCell *kc);

static inline struct ValueCellData*
get_data(ValueCell *kc)
{
    return (struct ValueCellData*)cell_get_data(value_cell_to_cell(kc));
}

static inline const struct ValueCellData*
get_data_const(const ValueCell *kc)
{
    return (const struct ValueCellData*)cell_get_data(
        value_cell_to_cell((ValueCell*)kc));
}

/* static Hive* */
/* get_hive(ValueCell *kc) */
/* { */
/*     return cell_get_hive(value_cell_to_cell(kc)); */
/* } */

static offset
get_offset(ValueCell *kc)
{
    return cell_get_offset(value_cell_to_cell(kc));
}


ValueCell *
value_cell_alloc (Hive *hdesc, offset rel,
                  const guint8 *data, int data_length)
{
    Cell *new_cell;
    ValueCell *ret_val;
    guint32 req_size;

    req_size = sizeof (guint32) + data_length;

    new_cell = cell_alloc (hdesc, rel, req_size);

    if (new_cell == NULL)
    {
        return NULL;
    }

    ret_val = (ValueCell*)new_cell;

    value_cell_set(ret_val, data, data_length);

    return ret_val;
}

gboolean
value_cell_set(ValueCell *in_vc, const guint8 *in_data, guint32 in_data_len)
{
    if (in_vc == NULL)
    {
        return FALSE;
    }

    if (in_data_len > value_cell_get_data_length(in_vc))
    {
        return FALSE;
    }

    // Really don't do this.  This is the cell's size not the data size.
    // And the cell is aligned on 8 byte boundaries.
    // get_data(ret_val)->size = req_size;
    cell_clear(value_cell_to_cell(in_vc));
    memcpy(get_data(in_vc)->data, in_data, in_data_len);

    return TRUE;
}


gboolean
value_cell_unalloc(ValueCell *vc)
{
    rra_info(N_("Unallocating (%#010x)"),
             offset_to_begin(cell_get_offset(value_cell_to_cell(vc))));
    return cell_unalloc(value_cell_to_cell(vc));
}

ValueCell *
value_cell_from_cell(Cell *c)
{
    ValueCell *ret_val = (ValueCell*)c;
    if (value_cell_is_valid(ret_val))
    {
        return ret_val;
    }
    else
    {
        return NULL;
    }
}

Cell *
value_cell_to_cell (ValueCell *vc)
{
    return (Cell*)vc;
}

gboolean
value_cell_is_valid (const ValueCell *vc)
{
    if (vc == NULL)
    {
        return FALSE;
    }

    // check for a sane size
    if (value_cell_get_data_length(vc) > 100000)
    {
        return FALSE;
    }

    return TRUE;
}

guint32
value_cell_get_data_length(const ValueCell *vc)
{
    guint32 tmp_size = abs(get_data_const(vc)->size);
    return tmp_size - sizeof(guint32);
}

guint8*
value_cell_get_data(ValueCell *vc)
{
    return get_data(vc)->data;
}

gboolean
value_cell_get_xml_output (const ValueCell *in_vc, ustring *in_output)
{
    if (in_vc == NULL || in_output == NULL)
    {
        return FALSE;
    }

    ValueCell *tmp_vc = (ValueCell*)in_vc;
    ustr_printfa(in_output,
                 "<ValueCell id=\"%#06x\">\n",
                 offset_to_begin(get_offset(tmp_vc)));
    ustr_printfa(in_output,
                 "  <data_length value=\"%u\" value_hex=\"%#010x\"/>\n",
                 value_cell_get_data_length(in_vc),
                 value_cell_get_data_length(in_vc));
    ustr_printfa(in_output,
                 "  <data>\n");
    ustr_hexdumpa(in_output,
                  (guint8*)get_data_const(in_vc)->data,
                  0, value_cell_get_data_length(in_vc), 1, FALSE);
    ustr_printfa(in_output,
                 "  </data>\n");
    ustr_printfa(in_output,
                 "</ValueCell>\n");
    return TRUE;
}

void
value_cell_debug_print (const ValueCell *vc)
{
    ustring *out = ustr_new();

    if (value_cell_get_xml_output(vc, out))
    {
        fprintf(stderr, "%s", ustr_as_utf8(out));
    }

    ustr_free(out);
}
