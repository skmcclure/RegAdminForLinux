/*
 * Authors:     James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006,2008 Racemi Inc
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

#include <glib/gi18n-lib.h>

#include <rregadmin/diff/value_key_cell.h>

#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/util/giochan_helpers.h>
#include <rregadmin/diff/log.h>
#include <rregadmin/diff/internal_defs.h>

int
value_key_cell_diff(rra_path *in_path,
                    ValueKeyCell *in_from, ValueKeyCell *in_to,
                    rra_diff_info *in_di)
{
    int ret_val = 0;

    if (in_from == NULL)
    {
        if (in_to == NULL)
        {
            return 0;
        }
        else
        {
            rra_di_add_value(in_di,
                             ustr_create(rra_path_as_str(in_path)),
                             value_key_cell_get_name(in_to),
                             value_key_cell_get_val(in_to));
            rra_di_set_compare_value(in_di, TO_GREATER);
            return TO_GREATER;
        }
    }
    else
    {
        if (in_to == NULL)
        {
            rra_di_delete_value(in_di,
                                ustr_create(rra_path_as_str(in_path)),
                                value_key_cell_get_name(in_from),
                                value_key_cell_get_val(in_from));
            rra_di_set_compare_value(in_di, FROM_GREATER);
            return FROM_GREATER;
        }
    }

    ustring *from_name = value_key_cell_get_name(in_from);
    ustring *to_name = value_key_cell_get_name(in_to);
    rra_info(N_("Comparing value key names: '%s' vs '%s'"),
             ustr_as_utf8(from_name), ustr_as_utf8(to_name));
    ret_val = ustr_compare(from_name, to_name);
    rra_di_set_compare_value(in_di, ret_val);
    ustr_free(from_name);
    ustr_free(to_name);

    if (ret_val > 0)
    {
        rra_di_add_value(in_di,
                         ustr_create(rra_path_as_str(in_path)),
                         value_key_cell_get_name(in_to),
                         value_key_cell_get_val(in_to));
    }
    else if (ret_val < 0)
    {
        rra_di_delete_value(in_di,
                            ustr_create(rra_path_as_str(in_path)),
                            value_key_cell_get_name(in_from),
                            value_key_cell_get_val(in_from));
    }
    else
    {
        Value *from_val = value_key_cell_get_val(in_from);
        Value *to_val = value_key_cell_get_val(in_to);

        ret_val = value_compare(from_val, to_val);
        rra_di_set_compare_value(in_di, ret_val);

        if (ret_val != 0)
        {
            rra_di_change_value(in_di,
                                ustr_create(rra_path_as_str(in_path)),
                                value_key_cell_get_name(in_from),
                                from_val, to_val);
        }
        else
        {
            value_free(to_val);
            value_free(from_val);
        }
    }

    return ret_val;
}
