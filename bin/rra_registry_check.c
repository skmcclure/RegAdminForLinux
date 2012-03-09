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

#include <rregadmin/rra_config.h>

#include <stdio.h>

#include <glib.h>

#include <rregadmin/basic.h>

#include <popt.h>

int
main (int argc, const char *argv[])
{
    gboolean got_version_request = FALSE;
    gboolean got_verbose_request = FALSE;

    const char **da_args = NULL;
    int da_arg_count = 0;
    char *prog_name = g_strdup(argv[0]);
    int exit_val = 0;

    rra_cli_init();
    rra_cli_init_from_env();

#ifdef RRA_HAVE_POPT_H
    struct poptOption entries[] =
        {
            {
                "version",
                'V',
                POPT_ARG_VAL,
                &got_version_request,
                TRUE,
                "Get version number",
                NULL
            },

            {
                "verbose",
                'v',
                POPT_ARG_VAL,
                &got_verbose_request,
                TRUE,
                "Set verbose output",
                NULL
            },

            POPT_AUTOHELP

            rra_util_options_get()[0],
            rra_cli_options_get()[0],

            POPT_TABLEEND
        };

    poptContext ctxt;

    ctxt = poptGetContext(NULL, argc, argv, entries, 0);

    poptSetOtherOptionHelp(
        ctxt, "[OPTIONS] <registry dir>");

    int rc;
    while ((rc = poptGetNextOpt(ctxt)) >= 0)
    {
    }

    da_args = poptGetArgs(ctxt);

    if (da_args != NULL)
    {
        for (da_arg_count = 0; da_args[da_arg_count] != NULL; da_arg_count++)
        {
        }
    }

#else
    da_args = argv + 1;
    da_arg_count = argc - 1;
#endif

    gchar *basename = g_path_get_basename(prog_name);
    if (got_version_request)
    {
        printf("Version for %s is %s\n", basename, RRA_VERSION);
        exit(0);
    }

    if (da_arg_count < 1)
    {
        fprintf (
            stderr,
            "Not enough arguments: %d\n"
            "Usage: %s [OPTIONS] <registry dir>\n",
            da_arg_count, basename);
        exit(1);
    }

    if (!g_file_test(da_args[0], G_FILE_TEST_IS_DIR))
    {
        fprintf (stderr, "Argument, %s, is not a directory\n",
                 da_args[0]);
        exit(1);
    }

    RRACliState *state = rra_cli_state_new_from_win_dir(da_args[0]);
    if (state == NULL)
    {
        fprintf (stderr, "Error creating basic state from %s\n",
                 da_args[0]);
        exit(1);
    }

    rra_cli_state_free(state);

    g_free(prog_name);

#if RRA_HAVE_POPT_H
    poptFreeContext(ctxt);
#endif

    exit(exit_val);
}
