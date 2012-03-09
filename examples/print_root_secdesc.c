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
    if (argc != 2)
    {
        fprintf (stderr, "Usage: %s <hive file>\n", argv[0]);
        exit(1);
    }

    Hive *hv = hive_read(argv[1]);
    if (hv == NULL)
    {
        return 1;
    }

    KeyCell *cell = hive_get_root_key(hv);
    const SecurityDescriptor *sd = key_cell_get_secdesc(cell);
    ustring *cell_name = key_cell_get_name(cell);
    ustring *hive_name = hive_get_name(hv);
    const HiveVersion *ver = hive_get_version(hv);

    if (sd == NULL)
    {
        printf ("Root of %s has no security descriptor\n",
                argv[1]);
    }
    else
    {
        printf ("File: %s\n", argv[1]);
        printf ("   HiveName: %s\n", ustr_as_utf8(hive_name));
        printf ("   HiveVersion: %d.%d.%d.%d\n",
                ver->major, ver->minor, ver->release, ver->build);
        printf ("   RootKeyName: %s\n", ustr_as_utf8(cell_name));
        printf ("%s\n", secdesc_to_sddl(sd));
    }

    ustr_free(hive_name);
    ustr_free(cell_name);
    hive_free(hv);

    return 0;
}
