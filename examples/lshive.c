/*
 * Authors:	James LewisMoss <jlm@racemi.com>
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

#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/util/path.h>

int
main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf (stderr, "Usage: %s <hive file> <path>\n", argv[0]);
        exit(1);
    }

    Hive *hv = hive_read(argv[1]);
    rra_path *path = rra_path_new_win(argv[2]);
    KeyCell *cell = hive_get_root_key(hv);
    int i;

    for (i = 0; i < rra_path_number_of_parts(path); i++)
    {
        cell = key_cell_get_subkey_ustr(cell, rra_path_get_part(path, i));
        if (cell == NULL)
        {
            fprintf (stderr, "Path %s doesn't exist\n",
                     rra_path_as_str(path));
            exit(1);
        }
    }

    key_cell_debug_print(cell);
    key_cell_pretty_print(cell, rra_path_as_str(path));

    rra_path_free(path);
    hive_free(hv);

    return 0;
}
