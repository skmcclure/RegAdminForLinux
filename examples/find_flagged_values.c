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

#include <stdio.h>
#include <string.h>

#include <glib.h>

#include <rregadmin/basic.h>
#include <rregadmin/registry/foreach.h>

static gboolean
check_for_flagged_value(KeyCell DECLARE_UNUSED(*in_kc),
                        ValueKeyCell *in_vkc,
                        const rra_path *in_path,
                        gpointer DECLARE_UNUSED(in_data))
{
    if ( value_key_cell_get_flags(in_vkc) != 1)
    {
        ustring *vkc_name = value_key_cell_get_name(in_vkc);
        rra_path *mod_path = rra_path_copy(in_path);
        rra_path_append_part(mod_path, ustr_as_utf8(vkc_name));
        value_key_cell_pretty_print(in_vkc, rra_path_as_str(mod_path));
        printf("\n------------------------------------------\n");
        rra_path_free(mod_path);
        ustr_free(vkc_name);
    }
    return TRUE;
}

int
main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf (stderr, "%s <win directory>\n", argv[0]);
        exit(1);
    }

    RRACliState *state = rra_cli_state_new_from_win_dir(argv[1]);

    if (state == NULL)
    {
        fprintf (stderr, "Couldn't load hives from %s\n", argv[1]);
        exit(1);
    }

    Registry *reg = rra_cli_state_get_registry(state);

    registry_foreach_value(reg, check_for_flagged_value, NULL);

    rra_cli_state_free(state);

    return 0;
}
