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

#include <rregadmin/registry/foreach.h>
#include <rregadmin/registry/search_order.h>
#include <rregadmin/registry/registry.h>
#include <rregadmin/registry/registry_key.h>

gboolean
registry_foreach_hive(Registry *in_hive,
                      RegistryForeachHiveFunc in_func,
                      gpointer in_data)
{
    if (in_hive == NULL
        || in_func == NULL)
    {
        return FALSE;
    }

    return FALSE;
}

gboolean
registry_foreach_key(Registry *in_reg, HiveForeachKeyFunc in_func,
                     gpointer in_data)
{
    if (in_reg == NULL || in_func == NULL)
    {
        return FALSE;
    }

    gboolean ret_val = TRUE;
    const char **search_item = hive_search_order;

    while (*search_item != NULL)
    {
        const RegistryKey *key =
            registry_find_registry_key(in_reg, *search_item);

        if (key != NULL)
        {
            rra_path *start_path = rra_path_new_win(*search_item);
            Hive* hv = registry_key_get_hive(key);
            if (!hive_foreach_key(hv, start_path, in_func, in_data))
            {
                ret_val = FALSE;
            }
            rra_path_free(start_path);
        }

        search_item++;
    }

    return ret_val;
}

gboolean
registry_foreach_value(Registry *in_reg, HiveForeachValueFunc in_func,
                       gpointer in_data)
{
    if (in_reg == NULL || in_func == NULL)
    {
        return FALSE;
    }

    gboolean ret_val = TRUE;
    const char **search_item = hive_search_order;

    while (*search_item != NULL)
    {
        const RegistryKey *key =
            registry_find_registry_key(in_reg, *search_item);

        if (key != NULL)
        {
            rra_path *start_path = rra_path_new_win(*search_item);
            Hive* hv = registry_key_get_hive(key);
            if (!hive_foreach_value(hv, start_path, in_func, in_data))
            {
                ret_val = FALSE;
            }
            rra_path_free(start_path);
        }

        search_item++;
    }

    return ret_val;
}
