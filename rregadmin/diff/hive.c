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

#include <rregadmin/diff/hive.h>

#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/util/giochan_helpers.h>
#include <rregadmin/diff/log.h>
#include <rregadmin/diff/internal_defs.h>
#include <rregadmin/diff/key_cell.h>

int
hive_version_compare(const HiveVersion *in_from_v, const HiveVersion *in_to_v)
{
    int ret_val;
    ret_val = in_from_v->major - in_to_v->major;

    if (ret_val == 0)
    {
        ret_val = in_from_v->minor - in_to_v->minor;
    }
    else
    {
        return ret_val;
    }

    if (ret_val == 0)
    {
        ret_val = in_from_v->release - in_to_v->release;
    }
    else
    {
        return ret_val;
    }

    if (ret_val == 0)
    {
        ret_val = in_from_v->build - in_to_v->build;
    }
    else
    {
        return ret_val;
    }

    return ret_val;
}

int
hive_diff(rra_path *in_path,
          Hive *in_from_hive, Hive *in_to_hive,
          rra_diff_info *in_di)
{
    if (in_to_hive == NULL)
    {
        if (in_from_hive == NULL)
        {
            return 0;
        }
        else
        {
            rra_di_set_compare_value(in_di, FROM_GREATER);
            return rra_di_get_compare_value(in_di);
        }
    }
    else
    {
        if (in_to_hive == NULL)
        {
            rra_di_set_compare_value(in_di, TO_GREATER);
            return rra_di_get_compare_value(in_di);
        }
    }

    rra_di_set_compare_value(
        in_di, hive_version_compare(hive_get_version(in_from_hive),
                                    hive_get_version(in_to_hive)));

    ustring *from_name = hive_get_name(in_from_hive);
    ustring *to_name = hive_get_name(in_to_hive);

    rra_info(N_("Comparing hives at path %s ('%s'/'%s' and '%s'/'%s')"),
             rra_path_as_str(in_path),
             hive_get_filename(in_from_hive), ustr_as_utf8(from_name),
             hive_get_filename(in_to_hive), ustr_as_utf8(to_name));

    rra_di_set_compare_value(in_di, ustr_compare(from_name, to_name));
    ustr_free(from_name);
    ustr_free(to_name);

    int key_name_comp = key_cell_compare_names(
        hive_get_root_key(in_from_hive), hive_get_root_key(in_to_hive));
    rra_di_set_compare_value(in_di, key_name_comp);

    int key_comp = key_cell_diff(in_path,
                                 hive_get_root_key(in_from_hive),
                                 hive_get_root_key(in_to_hive),
                                 in_di);
    rra_di_set_compare_value(in_di, key_comp);

    return rra_di_get_compare_value(in_di);
}

int
hive_compare(rra_path *in_path, Hive *in_from_hive, Hive *in_to_hive)
{
    rra_diff_info *di = rra_diff_info_new();
    int ret_val = hive_diff(in_path, in_from_hive, in_to_hive, di);
    rra_diff_info_free(di);
    return ret_val;
}

gboolean
hive_equal(rra_path *in_path, Hive *in_from_hive, Hive *in_to_hive)
{
    return hive_compare(in_path, in_from_hive, in_to_hive) == 0;
}
