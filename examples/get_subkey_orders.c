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

#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/util/path.h>

static void
do_subk_order(KeyCell *in_cell)
{
    guint32 i;

    if (key_cell_get_number_of_subkeys(in_cell) > 5)
    {
        ustring *subk_order = ustr_new();

        ustr_printf(subk_order,
                    "--------------------------------------------------\n");
        for (i = 0; i < key_cell_get_number_of_subkeys(in_cell); i++)
        {
            KeyCell *subk = key_cell_get_subkey(in_cell, i);
            ustring *subk_name = key_cell_get_name(subk);
            ustr_printfa(subk_order, "%s\n", ustr_as_utf8(subk_name));
            ustr_free(subk_name);
        }

        ustr_printfa(subk_order, "\n");

        printf(ustr_as_utf8(subk_order));
        ustr_free(subk_order);
    }

    for (i = 0; i < key_cell_get_number_of_subkeys(in_cell); i++)
    {
        KeyCell *subk = key_cell_get_subkey(in_cell, i);
        do_subk_order(subk);
    }
}

int
main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf (stderr, "Usage: %s <hive file>\n", argv[0]);
        exit(1);
    }

    Hive *hv = hive_read(argv[1]);

    KeyCell *cell = hive_get_root_key(hv);

    do_subk_order(cell);

    hive_free(hv);

    return 0;
}
