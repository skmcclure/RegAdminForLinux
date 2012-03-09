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
    gboolean got_help_command_request = FALSE;
    gboolean accept_pipe_commands = FALSE;

    const char **da_args = NULL;
    int da_arg_count = 0;
    char *prog_name = g_strdup(argv[0]);

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
                "help-commands",
                '\0',
                POPT_ARG_VAL,
                &got_help_command_request,
                TRUE,
                "Get help on available sub commands.",
                NULL
            },

            {
                "pipe-commands",
                'p',
                POPT_ARG_NONE,
                &accept_pipe_commands,
                TRUE,
                "Accept commands from stdin and write output to stdout.",
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
        ctxt, "[OPTIONS] <windows dir> <command> [<command args>]");

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

    if (got_help_command_request)
    {
        printf(
            "Available commands:\n%s",
            rra_cli_state_help_message(NULL));
        exit(0);
    }

    if (da_arg_count < 2)
    {
        fprintf (
            stderr,
            "Not enough arguments: %d\n"
            "Usage: %s [OPTIONS] <windows dir> [<arguments>]\n",
            da_arg_count, basename);
        exit(1);
    }

    if (!g_file_test(da_args[0], G_FILE_TEST_IS_DIR))
    {
        fprintf (stderr, "First argument, %s, is not a directory\n",
                 da_args[0]);
        exit(1);
    }

    RRACliState *state = rra_cli_state_new_from_win_dir(da_args[0]);

    if (state == NULL)
    {
        fprintf (stderr, "Error creating basic state\n");
        exit(1);
    }

    RRACliResult *result = rra_cli_state_apply(state, da_arg_count - 1,
                                               da_args + 1);
    int exit_val = 0;

    if (rra_cli_result_is_error(result))
    {
        if (rra_cli_result_has_content(result))
        {
            fprintf (stderr, "Error Encountered: %s\n",
                     rra_cli_result_get_content(result));
        }
        exit_val = 1;
    }
    else
    {
        if (rra_cli_result_has_data(result))
        {
            const GByteArray *data = rra_cli_result_get_data(result);
            if (data->len > 0)
            {
                fwrite(data->data, data->len, sizeof(guint8), stdout);
            }
        }
        else if (rra_cli_result_has_content(result))
        {
            printf ("%s\n", rra_cli_result_get_content(result));
        }
    }

    rra_cli_state_free(state);
    rra_cli_result_free(result);

    g_free(prog_name);

#if RRA_HAVE_POPT_H
    poptFreeContext(ctxt);
#endif

exit(exit_val);
}
