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

#include <stdio.h>
#include <locale.h>

#include <glib.h>

#include <rregadmin/basic.h>

#include "example_file.h"

static void
print_paths(KeyCell *in_kc, const char *in_path_start,
            example_file *in_ef)
{
    ef_line line;
    line.type = EF_VALUE;
    line.path = (char*)in_path_start;

    guint32 i;
    for (i = 0; i < key_cell_get_number_of_values(in_kc); i++)
    {
        ValueKeyCell *vkc = key_cell_get_value(in_kc, i);
        ustring *vkc_name = value_key_cell_get_name(vkc);
        line.name = (char*)ustr_as_utf8(vkc_name);
        ef_write_line(in_ef, &line);
        line.name = NULL;
        ustr_free(vkc_name);
    }

    for (i = 0; i < key_cell_get_number_of_subkeys(in_kc); i++)
    {
        KeyCell *next_kc = key_cell_get_subkey(in_kc, i);
        if (next_kc != NULL)
        {
            ustring *next_name = key_cell_get_name(next_kc);
            char *next_path_start =
                g_strdup_printf("%s\\%s", in_path_start,
                                ustr_as_utf8(next_name));
            ustr_free(next_name);

            print_paths(next_kc, next_path_start, in_ef);

            g_free(next_path_start);
        }
    }
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

    const char *start_paths[] = {
        "\\HKEY_LOCAL_MACHINE\\SAM",
        "\\HKEY_LOCAL_MACHINE\\system",
        "\\HKEY_LOCAL_MACHINE\\SECURITY",
        "\\HKEY_LOCAL_MACHINE\\software"
    };

    guint i;

    example_file *ef = ef_new(NULL, EF_VALUE);

    for (i = 0; i < G_N_ELEMENTS(start_paths); i++)
    {
        KeyCell *path_kc = registry_path_get_key(reg, start_paths[i]);
        if (path_kc == NULL)
            continue;
        print_paths(path_kc, start_paths[i], ef);
    }

    ef_free(ef);

    rra_cli_state_free(state);

    return 0;
}
