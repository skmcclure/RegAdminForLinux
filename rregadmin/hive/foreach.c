/*
 *
 * Authors:     James LewisMoss <jlm@racemi.com>
 *
 * Copyright 2006,2008 Racemi Inc
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

#include <rregadmin/hive/foreach.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>

static gboolean
hive_foreach_key_helper(KeyCell *in_kc,
                        rra_path *in_parent_path,
                        HiveForeachKeyFunc in_func,
                        gpointer in_data)
{
    guint i;
    gboolean ret_val = TRUE;

    if (!in_func(in_kc, in_parent_path, in_data))
    {
        return FALSE;
    }

    ustring *key_name = key_cell_get_name(in_kc);
    rra_path_append_part(in_parent_path, ustr_as_utf8(key_name));
    ustr_free(key_name);

    for (i = 0; i < key_cell_get_number_of_subkeys(in_kc); i++)
    {
        if (!hive_foreach_key_helper(key_cell_get_subkey(in_kc, i),
                                     in_parent_path, in_func, in_data))
        {
            ret_val = FALSE;
            break;
        }
    }

    rra_path_trim_back(in_parent_path, 1);

    return ret_val;
}

gboolean
hive_foreach_key(Hive *in_hive, const rra_path *in_start_path,
                 HiveForeachKeyFunc in_func, gpointer in_data)
{
    rra_path *da_path;
    if (in_start_path == NULL)
        da_path = rra_path_new_win("");
    else
        da_path = rra_path_copy(in_start_path);

    gboolean ret_val =
        hive_foreach_key_helper(hive_get_root_key(in_hive),
                                da_path, in_func, in_data);
    rra_path_free(da_path);

    return ret_val;
}

static gboolean
hive_foreach_value_helper(KeyCell *in_kc,
                          rra_path *in_parent_path,
                          HiveForeachValueFunc in_func,
                          gpointer in_data)
{
    guint i;
    gboolean ret_val = TRUE;

    ustring *key_name = key_cell_get_name(in_kc);
    rra_path_append_part(in_parent_path, ustr_as_utf8(key_name));
    ustr_free(key_name);

    for (i = 0; i < key_cell_get_number_of_values(in_kc); i++)
    {
        if (!in_func(in_kc, key_cell_get_value(in_kc, i), in_parent_path,
                     in_data))
        {
            ret_val = FALSE;
            break;
        }
    }

    if (ret_val)
    {
        for (i = 0; i < key_cell_get_number_of_subkeys(in_kc); i++)
        {
            if (!hive_foreach_value_helper(key_cell_get_subkey(in_kc, i),
                                           in_parent_path, in_func, in_data))
            {
                ret_val = FALSE;
                break;
            }
        }
    }

    rra_path_trim_back(in_parent_path, 1);

    return ret_val;
}

gboolean
hive_foreach_value(Hive *in_hive,
                   const rra_path *in_start_path,
                   HiveForeachValueFunc in_func,
                   gpointer in_data)
{
    rra_path *da_path;
    if (in_start_path == NULL)
        da_path = rra_path_new_win("");
    else
        da_path = rra_path_copy(in_start_path);

    gboolean ret_val =
        hive_foreach_value_helper(hive_get_root_key(in_hive),
                                  da_path, in_func, in_data);

    rra_path_free(da_path);

    return ret_val;
}
