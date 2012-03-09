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

#define _GNU_SOURCE
#include <stdio.h>

#include <glib.h>

#include <rregadmin/basic.h>

#include "example_file.h"

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

    example_file *inf = ef_open(NULL);
    example_file *outf = ef_new(NULL, EF_KEY);

    ef_line *line = NULL;

    for (line = ef_next(inf);
         line != NULL;
         line = ef_next(inf))
    {
        if(registry_path_is_key(reg, line->path))
        {
            ef_write_line(outf, line);
        }
        ef_line_free(line);
    }

    ef_free(inf);
    ef_free(outf);

    rra_cli_state_free(state);

    return 0;
}
