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

#include <rregadmin/diff/key_cell.h>

#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/util/giochan_helpers.h>
#include <rregadmin/diff/log.h>
#include <rregadmin/diff/internal_defs.h>
#include <rregadmin/diff/value_key_cell.h>

static int
local_ustr_compare(gconstpointer in1, gconstpointer in2,
                   gpointer DECLARE_UNUSED(data))
{
    /*
     * Must be case insensitive compare here.
     */
    return ustr_casecmp((const ustring*)in1, (const ustring*)in2);
}

#if 0
static gboolean
print_value_name_list_helper(gpointer key,
                             gpointer DECLARE_UNUSED(value),
                             gpointer DECLARE_UNUSED(data))
{
    fprintf (stderr, "\"%s\", ", ustr_as_utf8((ustring*)key));
    return FALSE;
}

static void
print_value_name_list(GTree *t)
{
    fprintf (stderr, "ValueNameList: [");
    g_tree_foreach(t, print_value_name_list_helper, NULL);
    fprintf (stderr, "]\n");
}
#endif

static int bogus_value = 1;

static void
add_val_names_from_key_cell(GTree *in_tree, KeyCell *in_kc)
{
    if (in_kc != NULL)
    {
        int i;
        int v_size = key_cell_get_number_of_values(in_kc);
        for (i = 0; i < v_size; i++)
        {
            ValueKeyCell *vkc = key_cell_get_value(in_kc, i);
            ustring *vkc_name = value_key_cell_get_name(vkc);
            if (g_tree_lookup(in_tree, vkc_name) == NULL)
            {
                g_tree_insert(in_tree, vkc_name, &bogus_value);
            }
            else
            {
                ustr_free(vkc_name);
            }
        }
    }
}

static GTree*
create_value_name_list(KeyCell *in_kc1, KeyCell *in_kc2)
{
    GTree *ret_val = g_tree_new_full(local_ustr_compare, NULL,
                                     (GDestroyNotify)ustr_free, NULL);

    add_val_names_from_key_cell(ret_val, in_kc1);
    add_val_names_from_key_cell(ret_val, in_kc2);

    return ret_val;
}

static void
destroy_value_name_list(GTree *in_list)
{
    g_tree_destroy(in_list);
}

struct key_cells_for_value
{
    KeyCell *from;
    KeyCell *to;
    rra_path *path;
    rra_diff_info *di;
    gboolean be_verbose;
};

static gboolean
compare_subvalues_helper(gpointer key, gpointer DECLARE_UNUSED(value),
                         gpointer data)
{
    struct key_cells_for_value *keys = (struct key_cells_for_value*)data;
    ustring *name = (ustring*)key;

    ValueKeyCell *from_subv =
        (keys->from
         ? key_cell_get_value_str(keys->from, ustr_as_utf8(name))
         : NULL);
    ValueKeyCell *to_subv =
        key_cell_get_value_str(keys->to, ustr_as_utf8(name));

    value_key_cell_diff(keys->path, from_subv, to_subv, keys->di);

    return FALSE;
}

static void
compare_subvalues(rra_path *in_path,
                  KeyCell *in_from, KeyCell *in_to,
                  rra_diff_info *in_di)
{
    rra_info(N_("Comparing subvalues at %s"), rra_path_as_str(in_path));

    struct key_cells_for_value keys;
    keys.from = in_from;
    keys.to = in_to;
    keys.path = in_path;
    keys.di = in_di;

    int to_count = key_cell_get_number_of_values(in_to);
    int from_count = (in_from
                      ? key_cell_get_number_of_values(in_from)
                      : 0);

    GTree *name_list = create_value_name_list(in_from, in_to);

    rra_di_set_compare_value(in_di, from_count - to_count);

    keys.be_verbose = (from_count != to_count);

    g_tree_foreach(name_list, compare_subvalues_helper, &keys);

    destroy_value_name_list(name_list);
}

static void
compare_subkeys(rra_path *in_path,
                KeyCell *in_from, KeyCell *in_to,
                rra_diff_info *in_di)
{
    rra_info(N_("Comparing subkeys at %s"), rra_path_as_str(in_path));

    int from_index = 0;
    int to_index = 0;

    int from_count = (in_from != NULL
                      ? key_cell_get_number_of_subkeys(in_from)
                      : 0);
    int to_count = key_cell_get_number_of_subkeys(in_to);

    rra_di_set_compare_value(in_di, from_count - to_count);

    if (from_count - to_count != 0)
    {
        rra_info(N_("from subkey count = %d ; to subkey count = %d"),
                 from_count, to_count);
    }

    while(TRUE)
    {
        rra_path *new_path = rra_path_copy(in_path);

        if (to_index >= to_count)
        {
            if (from_index >= from_count)
            {
                rra_path_free(new_path);
                break;
            }
            KeyCell *from_subk = key_cell_get_subkey(in_from, from_index);
            ustring *name = key_cell_get_name(from_subk);
            if (rra_path_append_part(new_path, ustr_as_utf8(name)))
            {
                key_cell_diff(new_path, from_subk, NULL, in_di);
            }
            ustr_free(name);
            from_index++;
        }
        else
        {
            KeyCell *to_subk = key_cell_get_subkey(in_to, to_index);

            if (in_from == NULL || from_index >= from_count)
            {
                ustring *name = key_cell_get_name(to_subk);
                if (rra_path_append_part(new_path, ustr_as_utf8(name)))
                {
                    key_cell_diff(new_path, NULL, to_subk, in_di);
                }
                ustr_free(name);
                to_index++;
            }
            else
            {
                KeyCell *from_subk = key_cell_get_subkey(in_from, from_index);

                int kc_name_comp = key_cell_compare_names(from_subk, to_subk);
                rra_di_set_compare_value(in_di, kc_name_comp);

                rra_debug("Name compare = %d", kc_name_comp);

                if (kc_name_comp > 0)
                {
                    ustring *name = key_cell_get_name(to_subk);
                    if (rra_path_append_part(new_path, ustr_as_utf8(name)))
                    {
                        key_cell_diff(new_path, NULL, to_subk, in_di);
                    }
                    ustr_free(name);
                    to_index++;
                }
                else if (kc_name_comp < 0)
                {
                    ustring *name = key_cell_get_name(from_subk);
                    if (rra_path_append_part(new_path, ustr_as_utf8(name)))
                    {
                        key_cell_diff(new_path, from_subk, NULL, in_di);
                    }
                    ustr_free(name);
                    from_index++;
                }
                else if (kc_name_comp == 0)
                {
                    ustring *name = key_cell_get_name(from_subk);
                    if (rra_path_append_part(new_path, ustr_as_utf8(name)))
                    {
                        key_cell_diff(new_path, from_subk, to_subk, in_di);
                    }
                    ustr_free(name);
                    from_index++;
                    to_index++;
                }
            }
        }

        rra_path_free(new_path);
    }
}

int
key_cell_diff(rra_path *in_path,
              KeyCell *in_from, KeyCell *in_to,
              rra_diff_info *in_di)
{
    rra_info(N_("Comparing keys at %s"), rra_path_as_str(in_path));

    if (in_from == NULL)
    {
        if (in_to == NULL)
        {
            return 0;
        }
        else
        {
            rra_di_add_key(
                in_di, ustr_create(rra_path_as_str(in_path)));
            rra_di_set_compare_value(in_di, TO_GREATER);
        }
    }
    else
    {
        if (in_to == NULL)
        {
            rra_di_delete_key(
                in_di, ustr_create(rra_path_as_str(in_path)));
            rra_di_set_compare_value(in_di, FROM_GREATER);
            return FROM_GREATER;
        }
    }

    compare_subvalues(in_path, in_from, in_to, in_di);
    compare_subkeys(in_path, in_from, in_to, in_di);

    return rra_di_get_compare_value(in_di);
}
