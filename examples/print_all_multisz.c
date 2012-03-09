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
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/util/path.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/value.h>
#include <rregadmin/hive/util.h>

static void
check_key_cell(KeyCell *in_cell)
{
    guint i;

    for (i = 0; i < key_cell_get_number_of_values(in_cell); i++)
    {
        ValueKeyCell *vkc = key_cell_get_value(in_cell, i);
        if (value_key_cell_get_type(vkc) == REG_MULTI_SZ)
        {
            value_key_cell_debug_print(vkc);
        }
    }

    for (i = 0; i < key_cell_get_number_of_subkeys(in_cell); i++)
    {
        check_key_cell(key_cell_get_subkey(in_cell, i));
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

    check_key_cell(hive_get_root_key(hv));

    hive_free(hv);

    return 0;
}
