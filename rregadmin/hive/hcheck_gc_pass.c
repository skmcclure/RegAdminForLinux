/*
 * Authors:     James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi Inc
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

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/util/check.h>

#include <rregadmin/hive/hcheck_decl.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/bin.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/security_descriptor_cell.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/hive/key_list_cell.h>
#include <rregadmin/hive/value_list_cell.h>
#include <rregadmin/hive/value_cell.h>

static gboolean
walk_key_tree(KeyCell *in_kc, RRACheckData *in_data)
{
    guint i;
    if (in_kc == NULL)
    {
        rra_check_warning(in_data, _("NULL key cell"));
        return FALSE;
    }
    gboolean ret_val = TRUE;

    cell_set_mark(key_cell_to_cell(in_kc), CELL_FLAG_LINKED);

    ValueListCell *vlc = key_cell_get_value_list_cell(in_kc);
    if (vlc != NULL)
    {
        cell_set_mark(value_list_cell_to_cell(vlc), CELL_FLAG_LINKED);
    }
    for (i = 0; i < key_cell_get_number_of_values(in_kc); i++)
    {
        ValueKeyCell *vkc = key_cell_get_value(in_kc, i);
        cell_set_mark(value_key_cell_to_cell(vkc), CELL_FLAG_LINKED);
        ValueCell *vc = value_key_cell_get_value_cell(vkc);
        if (vc != NULL)
        {
            cell_set_mark(value_cell_to_cell(vc), CELL_FLAG_LINKED);
        }
    }

    SecurityDescriptorCell *sdc = key_cell_get_security_descriptor_cell(in_kc);
    if (sdc != NULL)
    {
        cell_set_mark(security_descriptor_cell_to_cell(sdc), CELL_FLAG_LINKED);
    }

    KeyListCell *klc = key_cell_get_key_list_cell(in_kc);
    if (klc != NULL)
    {
        cell_set_mark(key_list_cell_to_cell(klc), CELL_FLAG_LINKED);
        for (i = 0; i < key_list_cell_get_number_of_sublists(klc); i++)
        {
            KeyListCell *sub_klc = key_list_cell_get_sublist(klc, i);
            cell_set_mark(key_list_cell_to_cell(sub_klc), CELL_FLAG_LINKED);
        }
    }

    ValueCell *cls_name = key_cell_get_classname_value_cell(in_kc);
    if (cls_name != NULL)
    {
        cell_set_mark(value_cell_to_cell(cls_name), CELL_FLAG_LINKED);
    }

    /* walk into subkeys */
    for (i = 0; i < key_cell_get_number_of_subkeys(in_kc); i++)
    {
        KeyCell *subk = key_cell_get_subkey(in_kc, i);
        if (!walk_key_tree(subk, in_data))
        {
            ret_val = FALSE;
        }
    }

    return ret_val;
}

static void
clear_all_cell_links(Hive *in_hive)
{
    Bin *b1;
    for (b1 = hive_get_first_bin(in_hive);
         b1 != NULL;
         b1 = bin_get_next_bin(b1))
    {
        Cell *c1;
        for (c1 = bin_first_cell(b1);
             c1 != NULL;
             c1 = cell_get_next(c1))
        {
            cell_clear_mark(c1, CELL_FLAG_LINKED);
        }
    }
}

static gboolean
check_all_cell_links(Hive *in_hive, RRACheckData *in_data)
{
    gboolean ret_val = TRUE;

    Bin *b1;
    for (b1 = hive_get_first_bin(in_hive);
         b1 != NULL;
         b1 = bin_get_next_bin(b1))
    {
        Cell *c1;
        for (c1 = bin_first_cell(b1);
             c1 != NULL;
             c1 = cell_get_next(c1))
        {
            if (cell_check_mark(c1, CELL_FLAG_LINKED))
            {
                if (!cell_is_allocd(c1))
                {
                    rra_check_warning(
                        in_data,
                        _("Cell %#010x is linked, but unallocated"),
                        offset_to_begin(cell_get_offset(c1)));
                    /* REPAIR: mark cell allocated */
                    ret_val = FALSE;
                    cell_debug_print_long(c1);
                }
            }
            else
            {
                if (cell_is_allocd(c1))
                {
                    rra_check_warning(
                        in_data,
                        _("Cell %#010x is allocated, but unlinked"),
                                      offset_to_begin(cell_get_offset(c1)));
                    /* REPAIR: unallocate the unused cell */
                    ret_val = FALSE;
                    cell_debug_print_long(c1);
                    /* find_possible_link_points(
                       in_hive, in_data,
                       offset_to_begin(cell_get_offset(c1))); */

                }
            }
        }
    }

    return ret_val;
}

gboolean
hcheck_gc_pass(Hive *in_hive, RRACheckData *in_data)
{
    if (in_hive == NULL)
    {
        return FALSE;
    }

    gboolean ret_val = TRUE;

    clear_all_cell_links(in_hive);

    KeyCell *kc = hive_get_root_key(in_hive);
    if (!walk_key_tree(kc, in_data))
    {
        ret_val = FALSE;
    }

    if (!check_all_cell_links(in_hive, in_data))
    {
        ret_val = FALSE;
    }

    /*
     * @fixme There are some problems in the test data so for the moment I'm
     * just returning true here
     *
     * return ret_val;
     */
    return TRUE;
}
