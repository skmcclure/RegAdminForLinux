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

#include <rregadmin/config.h>

#include <stdio.h>
#include <stdlib.h>

#include <glib/gi18n-lib.h>

#include <rregadmin/cli/options.h>
#include <rregadmin/cli/log.h>

static gboolean dont_make_backup = FALSE;
static gboolean hive_check_read = FALSE;
static gboolean hive_check_write = FALSE;

#ifdef RRA_HAVE_POPT_H
static struct poptOption entries[] =
{
    {
        "dont-make-backups",
        'b',
        POPT_ARG_NONE,
        &dont_make_backup,
        0,
        N_("Make backups of hives before write."),
        NULL
    },
    {
        "hive-check-read",
        'r',
        POPT_ARG_NONE,
        &hive_check_read,
        0,
        N_("Run a check on hive integrity when it is read."),
        NULL
    },
    {
        "hive-check-write",
        'w',
        POPT_ARG_NONE,
        &hive_check_write,
        0,
        N_("Run a check on hive integrity when it is written."),
        NULL
    },

    POPT_TABLEEND
};
static struct poptOption*
get_group(void)
{
    static struct poptOption ret_val[] = {
        {
            NULL,
            '\0',
            POPT_ARG_INCLUDE_TABLE,
            (void*)entries,
            0,
            N_("Options for the Basic API portion of the RRegAdmin library"),
            NULL
        },

        POPT_TABLEEND
    };

    return ret_val;
}
#endif

static const char *env_name = "RRA_CLI_OPTIONS";

gboolean
rra_cli_options_load_from_env(const char *env_var)
{
    gboolean ret_val = TRUE;
#ifdef RRA_HAVE_POPT_H
    const char *my_env = ((env_var == NULL) ? env_name : env_var);
    const char *env_opt = getenv(my_env);

    if (env_opt != NULL)
    {
        gchar *parse_env = NULL;
        gint argc= 0;
        gchar **argv = NULL;
        GError *error = NULL;

        parse_env = g_strdup_printf("ignore-program-name %s", env_opt);

        if (g_shell_parse_argv(parse_env, &argc, &argv, &error))
        {
            poptContext ctxt = poptGetContext(NULL,
                                              argc,
                                              (const char**)argv,
                                              get_group(),
                                              0);

            int rc;
            while ((rc = poptGetNextOpt(ctxt)) > 0)
            {
            }

            if (rc >= 0)
            {
                rra_warning(
                    _("Got > 0 return from poptGetNextOpt %s: %s: %d"),
                    env_name, parse_env, rc);
                ret_val = FALSE;
            }
            else if (rc != -1)
            {
                rra_warning(
                    _("Unable to parse options from %s: %s: %s"),
                    env_name, parse_env, poptStrerror(rc));
                ret_val = FALSE;
            }
            g_strfreev(argv);
            poptFreeContext(ctxt);
        }
        else
        {
            rra_warning(N_("Unable to split options from %s: %s"),
                        env_name,
                        (error != NULL
                         ? error->message
                         : _("No error message")));
            ret_val = FALSE;

            if (error != NULL)
            {
                g_error_free(error);
                error = NULL;
            }
        }

        g_free(parse_env);
    }
#endif
    return ret_val;
}

#ifdef RRA_HAVE_POPT_H
const struct poptOption*
rra_cli_options_get(void)
{
    return get_group();
}
#else
const void*
rra_cli_options_get(void)
{
    return NULL;
}
#endif

void
rra_cli_options_reset(void)
{
    dont_make_backup = FALSE;
    hive_check_read = FALSE;
    hive_check_write = FALSE;
}

gboolean
rra_cli_options_should_make_backup(void)
{
    return !dont_make_backup;
}

gboolean
rra_cli_options_should_check_hive_read(void)
{
    return hive_check_read;
}

gboolean
rra_cli_options_should_check_hive_write(void)
{
    return hive_check_write;
}
