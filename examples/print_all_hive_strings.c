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
    ustring *classname;

    ustring *kc_name = key_cell_get_name(in_cell);
    printf("%s\n", ustr_as_utf8(kc_name));
    ustr_free(kc_name);

    classname = key_cell_get_classname(in_cell);
    if (classname != NULL)
    {
        if (ustr_strlen(classname) > 0)
        {
            printf("%s\n", ustr_as_utf8(classname));
        }
        ustr_free(classname);
    }

    for (i = 0; i < key_cell_get_number_of_values(in_cell); i++)
    {
        ValueKeyCell *vkc = key_cell_get_value(in_cell, i);
        ustring *vkc_name = value_key_cell_get_name(vkc);
        printf("%s\n", ustr_as_utf8(vkc_name));
        ustr_free(vkc_name);

        Value *vkc_val = value_key_cell_get_val(vkc);
        if (value_is_string_type(vkc_val))
        {
            ustring *val_str = value_get_as_string(vkc_val);
            printf("%s\n", ustr_as_utf8(val_str));
            ustr_free(val_str);
        }
        value_free(vkc_val);
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

    ustring *hv_name = hive_get_name(hv);
    printf("%s\n", ustr_as_utf8(hv_name));
    ustr_free(hv_name);

    check_key_cell(hive_get_root_key(hv));

    hive_free(hv);

    return 0;
}
