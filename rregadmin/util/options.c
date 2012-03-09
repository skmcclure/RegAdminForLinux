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
#include <rregadmin/rra_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/util/options.h>
#include <rregadmin/util/log.h>

static gboolean should_debug_memory = FALSE;

static void
change_log_level (poptContext DECLARE_UNUSED(in_ctxt),
                  enum poptCallbackReason DECLARE_UNUSED(in_reason),
                  const struct poptOption *in_opt,
                  const char *in_arg,
                  void DECLARE_UNUSED(*data))
{
    int new_level = rra_log_get_level();

    if (in_opt->shortName == 'd')
    {
        new_level = G_LOG_LEVEL_DEBUG;
    }
    else if (in_opt->shortName == 'l')
    {
        new_level = rra_log_string_to_level(in_arg);

        if (new_level == 0)
        {
            rra_warning(N_("Unknown argument to -l option: %s"), in_arg);
        }
    }
    else
    {
        rra_warning(N_("Unhandled argument: %s"), in_opt->longName);
    }

    rra_log_set_level(new_level);
}

#ifdef RRA_HAVE_POPT_H
static const struct poptOption entries[] =
{
    {
        NULL,
        '\0',
        POPT_ARG_CALLBACK,
        change_log_level,
        0,
        NULL,
        NULL
    },

    {
        "debug",
        'd',
        POPT_ARG_NONE,
        NULL,
        G_LOG_LEVEL_DEBUG,
        N_("Turn on debug output."),
        NULL
    },

    {
        "log-level",
        'l',
        POPT_ARG_STRING,
        NULL,
        5,
        N_("Set the log level to a specific value."),
        "warning|message|info|debug"
    },

    {
        "debug-memory",
        '\0',
        POPT_ARG_NONE,
        &should_debug_memory,
        TRUE,
        N_("Turn on memory debugging."),
        NULL
    },

    POPT_TABLEEND
};

static const struct poptOption*
get_group(void)
{
    static struct poptOption ret_val[] = {
        {
            NULL,
            '\0',
            POPT_ARG_INCLUDE_TABLE,
            (void*)entries,
            0,
            N_("Options for the Util portion of the RRegAdmin library"),
            NULL
        }
    };

    return ret_val;
}
#endif

static const char *env_name = "RRA_UTIL_OPTIONS";

gboolean
rra_util_options_load_from_env(const char *env_var)
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
            char *arg_str = g_strjoinv(", ", argv);
            rra_info("(%d) %s", argc, arg_str);
            g_free(arg_str);

            poptContext ctxt = poptGetContext("",
                                              argc,
                                              (const char**)argv,
                                              get_group(),
                                              0);

            int rc;
            while ((rc = poptGetNextOpt(ctxt)) > 0)
            {
                printf("%d\n", rc);
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

            poptFreeContext(ctxt);
            g_strfreev(argv);
        }
        else
        {
            rra_warning(
                _("Unable to split options from %s: %s"),
                env_name,
                (error != NULL ? error->message : _("No error message")));
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
rra_util_options_get(void)
{
    return get_group();
}
#else
const void*
rra_util_options_get(void)
{
    return NULL;
}
#endif

void
rra_util_options_reset(void)
{
    rra_log_set_level(rra_log_get_default_level());
}

gboolean
rra_util_should_debug_memory(void)
{
    return should_debug_memory;
}
