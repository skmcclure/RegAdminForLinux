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

#include <glib/gi18n-lib.h>

#include <rregadmin/diff/registry.h>

#include <rregadmin/diff/hive.h>
#include <rregadmin/util/giochan_helpers.h>
#include <rregadmin/diff/log.h>
#include <rregadmin/registry/registry.h>
#include <rregadmin/registry/registry_key.h>
#include <rregadmin/registry/search_order.h>

rra_diff_info*
registry_diff(const Registry *in_reg1, const Registry *in_reg2)
{
    if (in_reg1 == NULL || in_reg2 == NULL)
    {
        return NULL;
    }

    rra_diff_info* ret_val = rra_diff_info_new();

    const char **search_item = hive_search_order;

    while (*search_item != NULL)
    {
        rra_info(N_("Comparing hives with path %s"), *search_item);

        const RegistryKey *key1 =
            registry_find_registry_key(in_reg1, *search_item);
        const RegistryKey *key2 =
            registry_find_registry_key(in_reg2, *search_item);

        if (key1 == NULL)
        {
            if (key2 != NULL)
            {
                rra_di_set_compare_value(ret_val, -1);
            }
        }
        else
        {
            if (key1 == NULL)
            {
                rra_di_set_compare_value(ret_val, 1);
            }
            else
            {
                Hive* hv1 = registry_key_get_hive(key1);
                Hive* hv2 = registry_key_get_hive(key2);
                rra_path *path = rra_path_new_win(*search_item);
                hive_diff(path, hv1, hv2, ret_val);
                rra_path_free(path);
            }
        }

        search_item++;
    }

    return ret_val;
}

int
registry_compare(const Registry *in_reg1, const Registry *in_reg2)
{
    rra_diff_info *di = registry_diff(in_reg1, in_reg2);
    int ret_val = rra_di_get_compare_value(di);
    rra_diff_info_free(di);
    return ret_val;
}

gboolean
registry_equal(const Registry *in_reg1, const Registry *in_reg2)
{
    return registry_compare(in_reg1, in_reg2) == 0;
}
