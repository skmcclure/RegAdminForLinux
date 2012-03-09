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
#include <stdlib.h>

#include <glib.h>

#include <rregadmin/secdesc/security_descriptor.h>

int
main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf (stderr, "Usage: %s <file>\n", argv[0]);
        exit(1);
    }

    gchar *contents;
    gsize length;
    GError *error;

    if (!g_file_get_contents(argv[1], &contents, &length, &error))
    {
        fprintf (stderr, "Unable to read file %s\n", argv[1]);
        exit(0);
    }


    SecurityDescriptor *sd =
        secdesc_new_parse_binary((guint8*)contents, (gsize*)&length);

    printf ("%s", secdesc_to_sddl(sd));

    return 0;
}
