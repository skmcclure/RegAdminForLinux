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
#include <strings.h>

#include <glib.h>

#include <rregadmin/basic.h>
#include <rregadmin/diff/registry.h>
#include <rregadmin/util/giochan_helpers.h>

#include <popt.h>

static const char* usage_str = "[OPTIONS] <registry dir 1> <registry dir 2>";

static void
print_error_usage(char *prog_name)
{
    fprintf (stderr, "%s %s\n", prog_name, usage_str);
}

typedef gchar* (*action_to_str_type)(const rra_diff_action *in_action);

static gchar*
action_to_str_default(const rra_diff_action *in_action)
{
    return rra_di_action_to_string(in_action);
}

static gchar*
action_to_str_regdiff(const rra_diff_action *in_action)
{
    switch(in_action->action)
    {
    case RRA_DI_ADD_KEY:
        return g_strdup_printf("[%s] (new)",
                               ustr_as_utf8((ustring*)in_action->data));
        break;

    case RRA_DI_DELETE_KEY:
        return g_strdup_printf("[%s] -",
                               ustr_as_utf8((ustring*)in_action->data));
        break;

    case RRA_DI_ADD_VALUE:
        {
            rra_di_value_action *va = (rra_di_value_action*)in_action->data;
            ustring *val_str = value_get_as_string(va->val_new);
            gchar *ret_val = g_strdup_printf(
                "[%s]\n"
                "\"%s\"=%s:%s     (new)",
                ustr_as_utf8(va->path), ustr_as_utf8(va->name),
                value_get_type_str(va->val_new), ustr_as_utf8(val_str));
            ustr_free(val_str);
            return ret_val;
        }
        break;

    case RRA_DI_DELETE_VALUE:
        {
            rra_di_value_action *va = (rra_di_value_action*)in_action->data;
            ustring *val_str = value_get_as_string(va->val_old);
            gchar *ret_val = g_strdup_printf(
                "[%s]\n"
                "\"%s\"",
                ustr_as_utf8(va->path), ustr_as_utf8(va->name));
            ustr_free(val_str);
            return ret_val;
        }
        break;

    case RRA_DI_CHANGE_VALUE:
        {
            rra_di_value_action *va = (rra_di_value_action*)in_action->data;
            ustring *val_old_str = value_get_as_string(va->val_old);
            ustring *val_new_str = value_get_as_string(va->val_new);
            gchar *ret_val = g_strdup_printf(
                "[%s]\n"
                "\"%s\"=%s:%s     (%s:%s)",
                ustr_as_utf8(va->path), ustr_as_utf8(va->name),
                value_get_type_str(va->val_old), ustr_as_utf8(val_old_str),
                value_get_type_str(va->val_new), ustr_as_utf8(val_new_str));
            ustr_free(val_old_str);
            ustr_free(val_new_str);
            return ret_val;
        }
        break;

    case RRA_DI_VERSION_CHANGE:
        return g_strdup_printf("HIVE_VERSION_CHANGE()");
        break;

    case RRA_DI_HIVE_NAME_CHANGE:
        return g_strdup_printf("HIVE_NAME_CHANGE(%s)",
                               ustr_as_utf8((ustring*)in_action->data));
        break;

    default:
        return g_strdup("Unknown action type");
    };
}

static gchar*
action_to_str_pretty(const rra_diff_action *in_action)
{
    switch(in_action->action)
    {
    case RRA_DI_ADD_KEY:
        return g_strdup_printf("AddKey: '%s'",
                               ustr_as_utf8((ustring*)in_action->data));
        break;

    case RRA_DI_DELETE_KEY:
        return g_strdup_printf("DeleteKey: '%s'",
                               ustr_as_utf8((ustring*)in_action->data));
        break;

    case RRA_DI_ADD_VALUE:
        {
            rra_di_value_action *va = (rra_di_value_action*)in_action->data;
            ustring *val_str = value_get_as_string(va->val_new);
            gchar *ret_val = g_strdup_printf(
                "AddValue: '%s' '%s'\n"
                "%s:%s",
                ustr_as_utf8(va->path), ustr_as_utf8(va->name),
                value_get_type_str(va->val_new), ustr_as_utf8(val_str));
            ustr_free(val_str);
            return ret_val;
        }
        break;

    case RRA_DI_DELETE_VALUE:
        {
            rra_di_value_action *va = (rra_di_value_action*)in_action->data;
            ustring *val_str = value_get_as_string(va->val_old);
            gchar *ret_val = g_strdup_printf(
                "DeleteValue: '%s' '%s'",
                ustr_as_utf8(va->path), ustr_as_utf8(va->name));
            ustr_free(val_str);
            return ret_val;
        }
        break;

    case RRA_DI_CHANGE_VALUE:
        {
            rra_di_value_action *va = (rra_di_value_action*)in_action->data;
            ustring *val_old_str = value_get_as_string(va->val_old);
            ustring *val_new_str = value_get_as_string(va->val_new);
            gchar *ret_val = g_strdup_printf(
                "ChangeValue: '%s' '%s'\n"
                "%s:%s\n(%s:%s)",
                ustr_as_utf8(va->path), ustr_as_utf8(va->name),
                value_get_type_str(va->val_old), ustr_as_utf8(val_old_str),
                value_get_type_str(va->val_new), ustr_as_utf8(val_new_str));
            ustr_free(val_old_str);
            ustr_free(val_new_str);
            return ret_val;
        }
        break;

    case RRA_DI_VERSION_CHANGE:
        return g_strdup_printf("HIVE_VERSION_CHANGE()");
        break;

    case RRA_DI_HIVE_NAME_CHANGE:
        return g_strdup_printf("HIVE_NAME_CHANGE(%s)",
                               ustr_as_utf8((ustring*)in_action->data));
        break;

    default:
        return g_strdup("Unknown action type");
    };
}

static gchar*
action_to_str_udiff(const rra_diff_action *in_action)
{
    switch(in_action->action)
    {
    case RRA_DI_ADD_KEY:
        return g_strdup_printf("+1K %s",
                               ustr_as_utf8((ustring*)in_action->data));
        break;

    case RRA_DI_DELETE_KEY:
        return g_strdup_printf("-1K %s",
                               ustr_as_utf8((ustring*)in_action->data));
        break;

    case RRA_DI_ADD_VALUE:
        {
            rra_di_value_action *va = (rra_di_value_action*)in_action->data;
            ustring *val_str = value_get_as_string(va->val_new);
            gchar *ret_val = g_strdup_printf(
                "+1V %s\n"
                "+2V %s\n"
                "+3V %s\n"
                "+4V %s",
                ustr_as_utf8(va->path), ustr_as_utf8(va->name),
                value_get_type_str(va->val_new), ustr_as_utf8(val_str));
            ustr_free(val_str);
            return ret_val;
        }
        break;

    case RRA_DI_DELETE_VALUE:
        {
            rra_di_value_action *va = (rra_di_value_action*)in_action->data;
            ustring *val_str = value_get_as_string(va->val_old);
            gchar *ret_val = g_strdup_printf(
                "-1V %s\n"
                "-2V %s",
                ustr_as_utf8(va->path), ustr_as_utf8(va->name));
            ustr_free(val_str);
            return ret_val;
        }
        break;

    case RRA_DI_CHANGE_VALUE:
        {
            rra_di_value_action *va = (rra_di_value_action*)in_action->data;
            ustring *val_old_str = value_get_as_string(va->val_old);
            ustring *val_new_str = value_get_as_string(va->val_new);
            gchar *ret_val = g_strdup_printf(
                "~1V %s\n"
                "~2V %s\n"
                "-3V %s\n"
                "-4V %s\n"
                "+5V %s\n"
                "+6V %s",
                ustr_as_utf8(va->path), ustr_as_utf8(va->name),
                value_get_type_str(va->val_new), ustr_as_utf8(val_new_str),
                value_get_type_str(va->val_old), ustr_as_utf8(val_old_str));
            ustr_free(val_old_str);
            ustr_free(val_new_str);
            return ret_val;
        }
        break;

    case RRA_DI_VERSION_CHANGE:
        return g_strdup_printf("HIVE_VERSION_CHANGE()");
        break;

    case RRA_DI_HIVE_NAME_CHANGE:
        return g_strdup_printf("HIVE_NAME_CHANGE(%s)",
                               ustr_as_utf8((ustring*)in_action->data));
        break;

    default:
        return g_strdup("Unknown action type");
    };
}

static action_to_str_type
get_print_func(const char *in_mode_name)
{
    if (in_mode_name == NULL)
    {
        return action_to_str_default;
    }
    else if (strcasecmp(in_mode_name, "regdiff") == 0)
    {
        return action_to_str_regdiff;
    }
    else if (strcasecmp(in_mode_name, "pretty") == 0)
    {
        return action_to_str_pretty;
    }
    else if (strcasecmp(in_mode_name, "udiff") == 0)
    {
        return action_to_str_udiff;
    }
    else
    {
        return action_to_str_default;
    }
}

int
main (int argc, const char *argv[])
{
    gboolean got_version_request = FALSE;
    gboolean got_verbose_request = TRUE;
    const char *mode_name = NULL;
    action_to_str_type print_func;

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

            {
                "quiet",
                'q',
                POPT_ARG_VAL,
                &got_verbose_request,
                FALSE,
                "Set quiet output",
                NULL
            },

            {
                "mode",
                'm',
                POPT_ARG_STRING,
                &mode_name,
                TRUE,
                "Set output mode.",
                "default|regdiff|pretty|udiff"
            },

            POPT_AUTOHELP

            rra_util_options_get()[0],
            rra_cli_options_get()[0],

            POPT_TABLEEND
        };

    poptContext ctxt;

    ctxt = poptGetContext(NULL, argc, argv, entries, 0);

    poptSetOtherOptionHelp(ctxt, usage_str);

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

    print_func = get_print_func(mode_name);

    gchar *basename = g_path_get_basename(prog_name);
    if (got_version_request)
    {
        printf("Version for %s is %s\n", basename, RRA_VERSION);
        exit(0);
    }

    if (da_arg_count < 2)
    {
        fprintf (stderr, "Not enough arguments: %d\n", argc);
        print_error_usage(basename);
        exit(1);
    }

    if (!g_file_test(da_args[0], G_FILE_TEST_IS_DIR))
    {
        fprintf (stderr, "First argument, %s, is not a directory\n",
                 da_args[0]);
        print_error_usage(basename);
        exit(1);
    }

    if (!g_file_test(da_args[1], G_FILE_TEST_IS_DIR))
    {
        fprintf (stderr, "Second argument, %s, is not a directory\n",
                 da_args[1]);
        print_error_usage(basename);
        exit(1);
    }

    RRACliState *state1 = rra_cli_state_new_from_win_dir(da_args[0]);
    if (state1 == NULL)
    {
        fprintf (stderr, "Error creating basic state from %s\n",
                 da_args[0]);
        print_error_usage(basename);
        exit(1);
    }
    RRACliState *state2 = rra_cli_state_new_from_win_dir(da_args[1]);
    if (state2 == NULL)
    {
        fprintf (stderr, "Error creating basic state from %s\n",
                 da_args[1]);
        print_error_usage(basename);
        rra_cli_state_free(state1);
        exit(1);
    }

    GIOChannel *chan = NULL;
    if (got_verbose_request)
    {
        chan = g_io_channel_unix_new(fileno(stdout));
    }

    Registry *reg1 = rra_cli_state_get_registry(state1);
    Registry *reg2 = rra_cli_state_get_registry(state2);

    rra_g_io_channel_printf(chan, NULL,
                            "Comparing two registries: %p vs %p\n",
                            reg1, reg2);

    rra_diff_info* di = registry_diff(reg1, reg2);

    if (di == NULL)
    {
        fprintf (stderr, "registry_diff returned NULL\n");
        print_error_usage(basename);
        exit_val = 1;
    }

    int i;
    for (i = 0; i < rra_di_action_count(di); i++)
    {
        gchar *out_str = print_func(rra_di_get_action(di, i));
        rra_g_io_channel_printf (chan, NULL, "%s\n", out_str);
        g_free(out_str);
    }

    rra_g_io_channel_printf(chan, NULL, "Complete registry compare\n\n\n");

    rra_cli_state_free(state1);
    rra_cli_state_free(state2);

    g_free(prog_name);

#if RRA_HAVE_POPT_H
    poptFreeContext(ctxt);
#endif

    exit(exit_val);
}
