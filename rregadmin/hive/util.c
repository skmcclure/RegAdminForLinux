/*
 *
 * NT Registry Hive access library, constants & structures
 *
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

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/hive/util.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/util/path.h>
#include <rregadmin/hive/log.h>
#include <rregadmin/util/macros.h>

KeyCell*
kc_trav_path_internal(KeyCell *in_kc, rra_path *in_path, gboolean in_create)
{
    int i;
    KeyCell *tnk = in_kc;

    for (i = 0; i < rra_path_number_of_parts(in_path); i++)
    {
        const ustring *tok = rra_path_get_part(in_path, i);
        rra_debug(N_("Looking for subkey %s"), ustr_as_utf8(tok));
        KeyCell *tmp_kc = key_cell_get_subkey_ustr (tnk, tok);

        if (tmp_kc == NULL)
        {
            if (in_create)
            {
                rra_debug(N_("Creating subkey %s"), ustr_as_utf8(tok));
                tmp_kc = key_cell_add_subkey(tnk, tok);
                if (tmp_kc == NULL)
                {
                    tnk = NULL;
                    break;
                }
            }
            else
            {
                rra_debug(N_("Didn't find subkey %s"), ustr_as_utf8(tok));
                tnk = NULL;
                break;
            }
        }
        tnk = tmp_kc;
    }

    return tnk;
}

KeyCell *
key_cell_trav_path (Hive *hdesc, KeyCell *nk, const char *in_path)
{
    KeyCell *tnk;
    rra_path *path = rra_path_new_win(in_path);
    rra_path_normalize(path);

    if (nk == NULL
        || rra_path_is_absolute(path))
    {
        tnk = hive_get_root_key(hdesc);
    }
    else
    {
        tnk = nk;
    }

    tnk = kc_trav_path_internal(tnk, path, FALSE);

    rra_path_free(path);

    return tnk;
}

ValueKeyCell *
value_key_cell_trav_path (Hive *hdesc, KeyCell *nk, const char *in_path,
                          const char *in_value_name)
{
    ValueKeyCell *ret_val = NULL;

    rra_path *path = rra_path_new_win(in_path);

    KeyCell *kc = key_cell_trav_path (hdesc, nk, rra_path_as_str(path));

    if (kc != NULL)
    {
        ValueKeyCell *vkc = key_cell_get_value_str (kc, in_value_name);
        if (vkc != NULL)
        {
            ret_val = vkc;
        }
    }

    rra_path_free(path);

    return ret_val;
}

static void
gen_kc_path_helper(rra_path *in_path, KeyCell *in_kc)
{
    if (in_kc != NULL)
    {
        ustring *name = key_cell_get_name(in_kc);
        rra_path_prepend_part(in_path, ustr_as_utf8(name));
        ustr_free(name);

        gen_kc_path_helper(in_path, key_cell_get_parent(in_kc));
    }
}

rra_path*
generate_key_cell_path(KeyCell *in_kc)
{
    rra_path *ret_val = rra_path_new_win(NULL);

    gen_kc_path_helper(ret_val, in_kc);

    return ret_val;
}
