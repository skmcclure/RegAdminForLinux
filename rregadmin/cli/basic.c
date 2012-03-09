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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/cli/basic.h>
#include <rregadmin/registry/registry.h>
#include <rregadmin/cli/options.h>
#include <rregadmin/cli/log.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/util/value.h>
#include <rregadmin/util/value_type.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/fs.h>
#include <rregadmin/util/intutils.h>
#include <rregadmin/util/conversion_utils.h>

/** Structure containing the description for a particular command.
 *
 * @internal
 *
 * @ingroup basic_cli_group
 */
struct rra_cli_cmd
{
    const char *name;
    HTBasicOperation op;
    const char *arg_text;
    const char *help_text;
};

/** Structure containing registry state for a group of commands.
 *
 * @internal
 *
 * @ingroup basic_cli_group
 */
struct RRACliState_
{
    Registry *reg;
    rra_path *path;
    struct rra_cli_cmd *ops;
    ustring *help_message;
};

static char *
make_args_string(int argc, const char **argv)
{
    switch(argc)
    {
    case 0:
        return g_strdup("");
    case 1:
        return g_strdup_printf("'%s'", argv[0]);
    case 2:
        return g_strdup_printf("'%s', '%s'", argv[0], argv[1]);
    case 3:
        return g_strdup_printf("'%s', '%s', '%s'", argv[0], argv[1], argv[2]);
    case 4:
        return g_strdup_printf("'%s', '%s', '%s', '%s'",
                               argv[0], argv[1], argv[2], argv[3]);
    case 5:
        return g_strdup_printf("'%s', '%s', '%s', '%s', '%s'",
                               argv[0], argv[1], argv[2], argv[3], argv[4]);
    case 6:
        return g_strdup_printf("'%s', '%s', '%s', '%s', '%s', '%s'",
                               argv[0], argv[1], argv[2], argv[3],
                               argv[4], argv[5]);
    default:
        {
            gchar **tmp_arr = g_new0(gchar*, argc + 1);
            int i;
            for (i = 0; i < argc; i++)
            {
                tmp_arr[i] = g_strdup_printf("'%s'", argv[i]);
            }
            gchar *ret_val = g_strjoinv(", ", tmp_arr);
            g_strfreev(tmp_arr);
            return ret_val;
        }
    }
}

struct rra_cli_cmd commands[] = {
    {
        "exit",
        rra_cli_exit,
        "",
        N_("Set the exit status and return.")
    },
    {
        "cd",
        rra_cli_cd,
        N_("<path>"),
        N_("Change current working key to <path>.")
    },
    {
        "has_key",
        rra_cli_has_key,
        N_("<path>"),
        N_("Check to see if key at <path> exists.")
    },
    {
        "show_key",
        rra_cli_show_key,
        N_("<path>"),
        N_("Show information about the key at <path>.")
    },
    {
        "show_key_parseable",
        rra_cli_show_key_parseable,
        N_("<path>"),
        N_("Print parseable information about the key at <path>.")
    },
    {
        "delete_key",
        rra_cli_delete_key,
        N_("<path>"),
        N_("Delete the key at <path>.")
    },
    {
        "is_deletable_key",
        rra_cli_is_deletable_key,
        N_("<path>"),
        N_("Check to see if the key at <path> is allowed to be deleted.")
    },
    {
        "add_key",
        rra_cli_add_key,
        N_("<path>"),
        N_("Add key at <path>.  If the key already exists this is successful.")
    },
    {
        "has_value",
        rra_cli_has_value,
        N_("<path> <value name>"),
        N_("Check to see if value <value name> at <path> exists.")
    },
    {
        "show_value",
        rra_cli_show_value,
        N_("<path> <value name>"),
        N_("Show in a humam friendly format the value <value name> at <path>.")
    },
    {
        "cat_value",
        rra_cli_cat_value,
        N_("<path> <value name>"),
        N_("Output the raw value of <value name> from <path>.")
    },
    {
        "delete_value",
        rra_cli_delete_value,
        N_("<path> <value name>"),
        N_("Delete the value <value name> at <path>.")
    },
    {
        "add_value",
        rra_cli_add_value,
        N_("<path> <value name> [ <value> | <type> <value> "
           "| <type> -F <filename> ]"),
        N_("Add a value.  With no arguments after <value name> create an "
           "empty value.  With one argument create a string value of <value>. "
           "With two arguments create a value of type <type> from the string "
           "in <value>.  With three arguments read the data from <filename> "
           "and create value of type <type>.")
    },
    {
        "modify_value",
        rra_cli_modify_value,
        N_("<sub command> <path> <value name> <byte number> <mask>"),
        N_("Modify value <value name> at <path> using one of the "
           "sub commands (clear_bit or set_bit) at byte <byte number> "
           "with mask <mask>.")
    },
    { NULL, NULL, NULL, NULL }
};

RRACliState*
rra_cli_state_new(void)
{
    RRACliState *ret_val = g_new0(RRACliState, 1);
    ret_val->path = rra_path_new_win("\\");
    ret_val->reg = registry_new();

    if (rra_cli_options_should_make_backup())
    {
        registry_set_option(ret_val->reg, REG_OPT_BACKUP);
    }
    if (rra_cli_options_should_check_hive_read())
    {
        registry_set_option(ret_val->reg, REG_OPT_CHECK_HIVES_READ);
    }
    if (rra_cli_options_should_check_hive_write())
    {
        registry_set_option(ret_val->reg, REG_OPT_CHECK_HIVES_WRITE);
    }

    ret_val->ops = commands;
    ret_val->help_message = NULL;
    return ret_val;
}

RRACliState*
rra_cli_state_new_from_win_dir(const char *in_dir)
{
    RRACliState *ret_val = rra_cli_state_new();

    if (rra_cli_state_load_win_dir(ret_val, in_dir))
    {
        return ret_val;
    }
    else
    {
        rra_cli_state_free(ret_val);
        return NULL;
    }
}

gboolean
rra_cli_state_free(RRACliState *in_state)
{
    if (in_state == NULL)
    {
        return FALSE;
    }

    rra_path_free(in_state->path);
    in_state->path = NULL;
    if (registry_is_dirty(in_state->reg))
    {
        registry_flush_all(in_state->reg);
    }

    registry_free(in_state->reg);
    in_state->reg = NULL;

    g_free(in_state);

    return TRUE;
}

static ustring *help_message = NULL;

const char *
rra_cli_state_help_message(RRACliState *in_state)
{
    if (in_state == NULL)
    {
        if (help_message == NULL)
        {
            help_message = ustr_new();
            const struct rra_cli_cmd *cur_cmd;
            for (cur_cmd = commands; cur_cmd->name != NULL; cur_cmd++)
            {
                ustr_printfa(help_message,
                             "%s %s\n\n", cur_cmd->name, cur_cmd->arg_text);
                ustr_wrapa(help_message,
                           cur_cmd->help_text,
                           "    ", 78);
                ustr_printfa(help_message, "\n\n");
            }
        }

        return ustr_as_utf8(help_message);
    }
    else
    {
        if (in_state->help_message == NULL)
        {
            in_state->help_message = ustr_new();
            const struct rra_cli_cmd *cur_cmd;
            for (cur_cmd = in_state->ops; cur_cmd->name != NULL; cur_cmd++)
            {
                ustr_printfa(in_state->help_message,
                             "%s %s\n\n", cur_cmd->name, cur_cmd->arg_text);
                ustr_wrapa(in_state->help_message,
                           cur_cmd->help_text,
                           "    ", 78);
                ustr_printfa(in_state->help_message, "\n\n");
            }
        }

        return ustr_as_utf8(in_state->help_message);
    }
}

gboolean
rra_cli_state_load_win_dir(RRACliState *in_state, const char *in_dir)
{
    return registry_load_win_dir(in_state->reg, in_dir);
}

Registry*
rra_cli_state_get_registry(RRACliState *in_state)
{
    return in_state->reg;
}

const rra_path*
rra_cli_state_get_path(const RRACliState *in_state)
{
    return in_state->path;
}

static HTBasicOperation
find_operation(const char *in_cmd, struct rra_cli_cmd *in_ops)
{
    int i = 0;
    while (in_ops[i].name != NULL)
    {
        if (strcmp(in_cmd, in_ops[i].name) == 0)
        {
            return in_ops[i].op;
        }
        i++;
    }

    return NULL;
}

RRACliResult*
rra_cli_state_apply(RRACliState *in_state, int argc, const char **argv)
{
    if (in_state == NULL)
    {
        RRACliResult *ret_val = rra_cli_result_new();
        rra_cli_result_add_error(ret_val, _("RRACliState is NULL"));
        return ret_val;
    }

    if (argc < 1)
    {
        RRACliResult *ret_val = rra_cli_result_new();
        rra_cli_result_add_error(ret_val, _("need at least one argument"));
        return ret_val;
    }

    char *arg_str = make_args_string(argc, argv);
    rra_info(N_("running: %s"), arg_str);
    g_free(arg_str);

    const char *command = argv[0];

    HTBasicOperation op = find_operation(command, in_state->ops);

    if (op == NULL)
    {
        RRACliResult *ret_val = rra_cli_result_new();
        rra_cli_result_add_error(ret_val,
                                   _("Couldn't find operation for %s"),
                                   command);
        return ret_val;
    }

    return op(in_state, argc - 1, argv + 1);
}

/** Take an input string for a path and massage it into a useful path
 * for an operation.
 *
 * If in_path is absolute return a normalized rra_path parsed from it.
 *
 * If the path is relative get the current path from in_state then apply
 * the new path to the current path.  Normalize again and return the rra_path.
 *
 * @internal
 *
 * @ingroup basic_cli_group
 */
static rra_path*
massage_path(RRACliState *in_state, const char *in_path)
{
    rra_path *target = rra_path_new_win(in_path);
    rra_path *ret_val;

    if (rra_path_is_absolute(target))
    {
        ret_val = target;
        target = NULL;
    }
    else
    {
        ret_val = rra_path_copy(in_state->path);
        rra_path_add(ret_val, target);
    }

    if (target != NULL)
    {
        rra_path_free(target);
    }

    return ret_val;
}

RRACliResult*
rra_cli_exit(RRACliState *in_state, int argc,
             const char DECLARE_UNUSED(**argv))
{
    RRACliResult *ret_val = rra_cli_result_new();

    if (in_state == NULL)
    {
        rra_cli_result_add_error(ret_val, _("RRACliState is NULL"));
        return ret_val;
    }

    if (argc != 0)
    {
        rra_cli_result_add_error(
            ret_val, _("exit command takes no arguments."));
        return ret_val;
    }

    rra_cli_result_set_exit(ret_val);

    return ret_val;
}

RRACliResult*
rra_cli_cd(RRACliState *in_state, int argc, const char **argv)
{
    RRACliResult *ret_val = rra_cli_result_new();

    char *arg_str = make_args_string(argc, argv);
    rra_info(N_("running: %s"), arg_str);
    g_free(arg_str);

    if (in_state == NULL)
    {
        rra_cli_result_add_error(ret_val, _("RRACliState is NULL"));
        return ret_val;
    }

    if (argc != 1)
    {
        rra_cli_result_add_error(
            ret_val, _("cd command takes one and only one argument"));
        return ret_val;
    }

    rra_path *test_path = massage_path(in_state, argv[0]);

    if (registry_path_is_key(in_state->reg, rra_path_as_str(test_path)))
    {
        rra_path_set(in_state->path, test_path);
        rra_cli_result_set_success(ret_val);
        rra_cli_result_set_cell(
            ret_val,
            key_cell_to_cell(
                registry_path_get_key(
                    in_state->reg, rra_path_as_str(test_path))));
    }
    else
    {
        rra_cli_result_add_error(
            ret_val, _("key %s doesn't exist"), rra_path_as_str(test_path));
    }

    rra_path_free(test_path);

    return ret_val;

}

RRACliResult*
rra_cli_has_key(RRACliState *in_state, int argc, const char **argv)
{
    RRACliResult *ret_val = rra_cli_result_new();

    char *arg_str = make_args_string(argc, argv);
    rra_info(N_("running: %s"), arg_str);
    g_free(arg_str);

    if (in_state == NULL)
    {
        rra_cli_result_add_error(ret_val, _("RRACliState is NULL"));
        return ret_val;
    }

    if (argc != 1)
    {
        rra_cli_result_add_error(
            ret_val, _("has_key command takes one and only one argument"));
        return ret_val;
    }

    rra_path *test_path = massage_path(in_state, argv[0]);

    gboolean iskey =
        registry_path_is_key(rra_cli_state_get_registry(in_state),
                             rra_path_as_str(test_path));
    if (iskey)
    {
        rra_cli_result_set_success(ret_val);
        rra_cli_result_set_cell(
            ret_val,
            key_cell_to_cell(
                registry_path_get_key(
                    in_state->reg, rra_path_as_str(test_path))));
    }
    else
    {
        rra_cli_result_set_error(ret_val);
    }

    rra_path_free(test_path);

    return ret_val;
}

RRACliResult*
rra_cli_show_key_parseable(RRACliState *in_state, int argc,
                             const char **argv)
{
    RRACliResult *ret_val = rra_cli_result_new();

    char *arg_str = make_args_string(argc, argv);
    rra_info(N_("running: %s"), arg_str);
    g_free(arg_str);

    if (in_state == NULL)
    {
        rra_cli_result_add_error(ret_val, _("RRACliState is NULL"));
        return ret_val;
    }

    if (argc > 1)
    {
        rra_cli_result_add_error(
            ret_val, _("show_key command takes zero or one arguments"));
        return ret_val;
    }

    rra_path *test_path;
    if (argc == 0)
    {
        test_path = rra_path_copy(in_state->path);
    }
    else
    {
        test_path = massage_path(in_state, argv[0]);
    }

    KeyCell *cell =
        registry_path_get_key(rra_cli_state_get_registry(in_state),
                              rra_path_as_str(test_path));

    if (cell == NULL)
    {
        rra_cli_result_add_error(
            ret_val, _("No key at %s"), rra_path_as_str(test_path));
    }
    else
    {
        ustring *po = ustr_new();
        key_cell_get_parseable_output(cell, rra_path_as_str(test_path), po);
        rra_cli_result_add_message(ret_val, ustr_as_utf8(po));
        ustr_free(po);
        rra_cli_result_set_cell(ret_val, key_cell_to_cell(cell));
    }

    rra_path_free(test_path);

    return ret_val;
}

RRACliResult*
rra_cli_show_key(RRACliState *in_state, int argc, const char **argv)
{
    RRACliResult *ret_val = rra_cli_result_new();

    char *arg_str = make_args_string(argc, argv);
    rra_info(N_("running: %s"), arg_str);
    g_free(arg_str);

    if (in_state == NULL)
    {
        rra_cli_result_add_error(ret_val, _("RRACliState is NULL"));
        return ret_val;
    }

    if (argc > 1)
    {
        rra_cli_result_add_error(
            ret_val, _("show_key command takes zero or one arguments"));
        return ret_val;
    }

    rra_path *test_path;
    if (argc == 0)
    {
        test_path = rra_path_copy(in_state->path);
    }
    else
    {
        test_path = massage_path(in_state, argv[0]);
    }

    KeyCell *cell =
        registry_path_get_key(rra_cli_state_get_registry(in_state),
                              rra_path_as_str(test_path));

    if (cell == NULL)
    {
        rra_cli_result_add_error(
            ret_val, _("No key at %s"), rra_path_as_str(test_path));
    }
    else
    {
        ustring *po = ustr_new();
        key_cell_get_pretty_output(cell, rra_path_as_str(test_path), po);
        rra_cli_result_add_message(ret_val, ustr_as_utf8(po));
        ustr_free(po);
        rra_cli_result_set_cell(ret_val, key_cell_to_cell(cell));
    }

    rra_path_free(test_path);

    return ret_val;
}

RRACliResult*
rra_cli_delete_key(RRACliState *in_state, int argc, const char **argv)
{
    RRACliResult *ret_val = rra_cli_result_new();

    char *arg_str = make_args_string(argc, argv);
    rra_info(N_("running: %s"), arg_str);
    g_free(arg_str);

    if (in_state == NULL)
    {
        rra_cli_result_add_error(ret_val, _("RRACliState is NULL"));
        return ret_val;
    }

    if (argc != 1)
    {
        rra_cli_result_add_error(
            ret_val, _("delete_key command takes one and only one argument"));
        return ret_val;
    }

    rra_path *test_path = massage_path(in_state, argv[0]);

    gboolean is_done =
        registry_path_delete_key(rra_cli_state_get_registry(in_state),
                                 rra_path_as_str(test_path));

    if (is_done)
    {
        rra_cli_result_set_success(ret_val);
    }
    else
    {
        rra_cli_result_add_error(ret_val, _("Unable to delete key %s"),
                                   rra_path_as_str(test_path));
    }

    rra_path_free(test_path);

    return ret_val;
}

RRACliResult*
rra_cli_is_deletable_key(RRACliState *in_state, int argc, const char **argv)
{
    RRACliResult *ret_val = rra_cli_result_new();

    char *arg_str = make_args_string(argc, argv);
    rra_info(N_("running: %s"), arg_str);
    g_free(arg_str);

    if (in_state == NULL)
    {
        rra_cli_result_add_error(ret_val, _("RRACliState is NULL"));
        return ret_val;
    }

    if (argc != 1)
    {
        rra_cli_result_add_error(
            ret_val,
            _("is_deletable_key command takes one and only one argument"));
        return ret_val;
    }

    rra_path *test_path = massage_path(in_state, argv[0]);

    gboolean is_done =
        registry_path_is_deletable_key(rra_cli_state_get_registry(in_state),
                                       rra_path_as_str(test_path));

    if (is_done)
    {
        rra_cli_result_set_success(ret_val);
    }
    else
    {
        rra_cli_result_set_error(ret_val);
    }

    rra_path_free(test_path);

    return ret_val;
}

RRACliResult*
rra_cli_add_key(RRACliState *in_state, int argc, const char **argv)
{
    RRACliResult *ret_val = rra_cli_result_new();

    char *arg_str = make_args_string(argc, argv);
    rra_info(N_("running: %s"), arg_str);
    g_free(arg_str);

    if (in_state == NULL)
    {
        rra_cli_result_add_error(ret_val, _("RRACliState is NULL"));
        return ret_val;
    }

    if (argc != 1)
    {
        rra_cli_result_add_error(
            ret_val, _("add_key command takes one and only one argument"));
        return ret_val;
    }

    rra_path *test_path = massage_path(in_state, argv[0]);

    if (registry_path_is_key(rra_cli_state_get_registry(in_state),
                             rra_path_as_str(test_path)))
    {
        rra_cli_result_set_success(ret_val);
    }
    else
    {
        KeyCell *cell =
            registry_path_add_key(rra_cli_state_get_registry(in_state),
                                  rra_path_as_str(test_path));
        if (cell != NULL)
        {
            rra_cli_result_set_success(ret_val);
            rra_cli_result_set_cell(ret_val, key_cell_to_cell(cell));
        }
        else
        {
            rra_cli_result_set_error(ret_val);
        }
    }

    rra_path_free(test_path);

    return ret_val;
}

RRACliResult*
rra_cli_has_value(RRACliState *in_state, int argc, const char **argv)
{
    RRACliResult *ret_val = rra_cli_result_new();

    char *arg_str = make_args_string(argc, argv);
    rra_info(N_("running: %s"), arg_str);
    g_free(arg_str);

    if (in_state == NULL)
    {
        rra_cli_result_add_error(ret_val, _("RRACliState is NULL"));
        return ret_val;
    }

    if (argc != 2)
    {
        rra_cli_result_add_error(
            ret_val,
            _("has_value command takes two arguments: <path> <value name>"));
        return ret_val;
    }

    rra_path *test_path = massage_path(in_state, argv[0]);

    gboolean isval =
        registry_path_is_value(rra_cli_state_get_registry(in_state),
                               rra_path_as_str(test_path),
                               argv[1]);
    if (isval)
    {
        rra_cli_result_set_success(ret_val);
        rra_cli_result_set_cell(
            ret_val,
            value_key_cell_to_cell(
                registry_path_get_value(
                    in_state->reg, rra_path_as_str(test_path), argv[1])));
    }
    else
    {
        rra_cli_result_set_error(ret_val);
    }


    rra_path_free(test_path);

    return ret_val;
}

RRACliResult*
rra_cli_cat_value(RRACliState *in_state, int argc, const char **argv)
{
    RRACliResult *ret_val = rra_cli_result_new();

    char *arg_str = make_args_string(argc, argv);
    rra_info(N_("running: %s"), arg_str);
    g_free(arg_str);

    if (in_state == NULL)
    {
        rra_cli_result_add_error(ret_val, _("RRACliState is NULL"));
        return ret_val;
    }

    if (argc != 2)
    {
        rra_cli_result_add_error(
            ret_val,
            _("cat_value command takes two arguments: <path> <value name>"));
        return ret_val;
    }

    rra_path *test_path = massage_path(in_state, argv[0]);

    ValueKeyCell *vkc =
        registry_path_get_value(rra_cli_state_get_registry(in_state),
                                rra_path_as_str(test_path), argv[1]);

    if (vkc == NULL)
    {
        rra_cli_result_add_error(
            ret_val, _("No value at %s"), rra_path_as_str(test_path));
    }
    else
    {
        Value *val = value_key_cell_get_val(vkc);
        if (val == NULL)
        {
            rra_cli_result_add_error(ret_val, _("Couldn't get value"));
        }
        else
        {
            rra_cli_result_set_data(ret_val, value_encode(val, 0));
            value_free(val);
            rra_cli_result_set_cell(ret_val, value_key_cell_to_cell(vkc));
        }
    }

    rra_path_free(test_path);

    return ret_val;
}


RRACliResult*
rra_cli_show_value(RRACliState *in_state, int argc, const char **argv)
{
    RRACliResult *ret_val = rra_cli_result_new();

    char *arg_str = make_args_string(argc, argv);
    rra_info(N_("running: %s"), arg_str);
    g_free(arg_str);

    if (in_state == NULL)
    {
        rra_cli_result_add_error(ret_val, _("RRACliState is NULL"));
        return ret_val;
    }

    if (argc != 2)
    {
        rra_cli_result_add_error(
            ret_val,
            _("show_value command takes two arguments: <path> <value name>"));
        return ret_val;
    }

    rra_path *test_path = massage_path(in_state, argv[0]);

    ValueKeyCell *vkc =
        registry_path_get_value(rra_cli_state_get_registry(in_state),
                                rra_path_as_str(test_path), argv[1]);

    if (vkc == NULL)
    {
        rra_cli_result_add_error(
            ret_val, _("No value at %s"), rra_path_as_str(test_path));
    }
    else
    {
        ustring *po = ustr_new();
        value_key_cell_get_pretty_output(vkc, rra_path_as_str(test_path), po);
        rra_cli_result_add_message(ret_val, ustr_as_utf8(po));
        ustr_free(po);
        rra_cli_result_set_cell(ret_val, value_key_cell_to_cell(vkc));
    }

    rra_path_free(test_path);

    return ret_val;
}

RRACliResult*
rra_cli_delete_value(RRACliState *in_state, int argc, const char **argv)
{
    RRACliResult *ret_val = rra_cli_result_new();

    char *arg_str = make_args_string(argc, argv);
    rra_info(N_("running: %s"), arg_str);
    g_free(arg_str);

    if (in_state == NULL)
    {
        rra_cli_result_add_error(ret_val, _("RRACliState is NULL"));
        return ret_val;
    }

    if (argc != 2)
    {
        rra_cli_result_add_error(
            ret_val,
            _("delete_value command takes two arguments: "
              "<path> <value name>"));
        return ret_val;
    }

    rra_path *test_path = massage_path(in_state, argv[0]);

    gboolean is_done =
        registry_path_delete_value(rra_cli_state_get_registry(in_state),
                                   rra_path_as_str(test_path), argv[1]);

    if (is_done)
    {
        rra_cli_result_set_success(ret_val);
    }
    else
    {
        rra_cli_result_set_error(ret_val);
    }

    rra_path_free(test_path);

    return ret_val;
}

static Value*
create_value(RRACliResult *ret_val, const char *in_type,
             const char *in_value)
{
    if (in_type == NULL || in_value == NULL)
    {
        return NULL;
    }

    ValueType type = value_type_from_string(in_type);

    if (type == -1)
    {
        rra_cli_result_add_error(
            ret_val, _("add_value: invalid type string: %s"), in_type);
        return NULL;
    }

    /* Specially handle dword and qword here. */
    if (type == REG_DWORD)
    {
        if (strlen(in_value) == 0)
        {
            return NULL;
        }

        dword_type val;
        if (!fuzzy_str_to_dword(in_value, &val))
        {
            return NULL;
        }
        return value_create_dword(val);
    }
    else if (type == REG_QWORD)
    {
        if (strlen(in_value) == 0)
        {
            return NULL;
        }

        qword_type val;
        if (!fuzzy_str_to_qword(in_value, &val))
        {
            return NULL;
        }
        return value_create_qword(val);
    }
    else
    {
        return value_create_from_string(in_value, type);
    }
}

static Value*
create_value_from_file(RRACliResult *ret_val,
                       const char *in_type, const char *in_filename)
{
    if (in_type == NULL || in_filename == NULL)
    {
        return NULL;
    }

    ValueType type = value_type_from_string(in_type);

    if (type == -1)
    {
        rra_cli_result_add_error(
            ret_val, _("add_value: invalid type string: %s"), in_type);
        return NULL;
    }

    gchar *file_contents;
    gsize file_length;
    GError *file_error;

    if (!g_file_get_contents(in_filename, &file_contents,
                             &file_length, &file_error))
    {
        rra_cli_result_add_error(
            ret_val, _("add_value: error loading from file %s"), in_filename);
        return NULL;
    }

    Value *val = value_create_from_binary((guint8*)file_contents,
                                          file_length,
                                          type);
    g_free(file_contents);

    return val;
}

static Value*
generate_value(RRACliResult *in_result, int argc, const char **argv)
{
    Value *ret_val = NULL;

    switch(argc)
    {
    case 0:
        rra_info(N_("Generate value from no args"));
        // empty value case
        ret_val = value_create_none();
        break;

    case 1:
        rra_info(N_("Generate value from one arg: '%s'"), argv[0]);
        // simple string case
        ret_val = value_create_string(argv[0], strlen(argv[0]),
                                      USTR_TYPE_UTF8);
        break;

    case 2:
        rra_info(N_("Generate value from two args: '%s', '%s'"),
                 argv[0], argv[1]);
        // typed value case
        ret_val = create_value(in_result, argv[0], argv[1]);
        break;

    case 3:
        // value read from file case
        if (strcmp(argv[1], "-F") != 0)
        {
            rra_cli_result_add_error(
                in_result,
                _("add_value: Required option -F for read from "
                  "file not given"));
        }
        else
        {
            rra_info(N_("Generate value from arg -F arg: '%s', '%s'"),
                     argv[0], argv[2]);
            ret_val = create_value_from_file(in_result, argv[0], argv[2]);
            break;
        }

    default:
        // Nothing else allowed
        rra_cli_result_add_error(
            in_result,
            _("add_value: Invalid number of arguments: %d"), argc);
        break;
    }

    return ret_val;
}

RRACliResult*
rra_cli_add_value(RRACliState *in_state, int argc, const char **argv)
{
    RRACliResult *ret_val = rra_cli_result_new();

    char *arg_str = make_args_string(argc, argv);
    rra_info(N_("running: %s"), arg_str);
    g_free(arg_str);

    if (in_state == NULL)
    {
        rra_cli_result_add_error(ret_val, _("RRACliState is NULL"));
        return ret_val;
    }

    if (argc < 2)
    {
        rra_cli_result_add_error(
            ret_val,
            _("add_value command takes at least two arguments: "
              "<path> <value name>"));
        return ret_val;
    }

    Value *val = generate_value(ret_val, argc - 2, argv + 2);

    if (val == NULL)
    {
        rra_cli_result_add_error(ret_val, _("No value generated"));
        return ret_val;
    }

    rra_path *test_path = massage_path(in_state, argv[0]);

    ValueKeyCell *cell =
        registry_path_get_value(rra_cli_state_get_registry(in_state),
                                rra_path_as_str(test_path), argv[1]);

    if (cell != NULL)
    {
        if (value_key_cell_set_val(cell, val))
        {
            rra_cli_result_set_success(ret_val);
            rra_cli_result_set_cell(ret_val, value_key_cell_to_cell(cell));
        }
        else
        {
            rra_cli_result_set_error(ret_val);
        }
    }
    else
    {
        cell = registry_path_add_value(
            rra_cli_state_get_registry(in_state), rra_path_as_str(test_path),
            argv[1],
            val);

        if (cell != NULL)
        {
            rra_cli_result_set_success(ret_val);
            rra_cli_result_set_cell(ret_val, value_key_cell_to_cell(cell));
        }
        else
        {
            rra_cli_result_set_error(ret_val);
        }
    }

    value_free(val);
    rra_path_free(test_path);

    return ret_val;
}

struct BitRegData
{
    guint32 byte;
    guint8 mask;
};

static Value*
clear_bit_vkcmodder(Value *in_val, gpointer in_data)
{
    if (in_val == NULL)
    {
        return NULL;
    }

    if (in_data == NULL)
    {
        value_free(in_val);
        return NULL;
    }

    struct BitRegData *data = (struct BitRegData*)in_data;

    gboolean bit_op_ret = value_clear_bits(in_val, data->byte, data->mask);

    if (bit_op_ret)
    {
        return in_val;
    }
    else
    {
        value_free(in_val);
        return NULL;
    }
}

static Value*
set_bit_vkcmodder(Value *in_val, gpointer in_data)
{
    if (in_val == NULL)
    {
        return NULL;
    }

    if (in_data == NULL)
    {
        value_free(in_val);
        return NULL;
    }

    struct BitRegData *data = (struct BitRegData*)in_data;

    gboolean bit_op_ret = value_set_bits(in_val, data->byte, data->mask);

    if (bit_op_ret)
    {
        return in_val;
    }
    else
    {
        value_free(in_val);
        return NULL;
    }
}

RRACliResult*
rra_cli_modify_value(RRACliState *in_state, int argc, const char **argv)
{
    RRACliResult *ret_val = rra_cli_result_new();

    const char *arg_op = argv[0];
    const char *arg_path = argv[1];
    const char *arg_val_name = argv[2];
    const char *arg_byte_num = argv[3];
    const char *arg_byte_mask = argv[4];

    char *arg_str = make_args_string(argc, argv);
    rra_info(N_("running: %s"), arg_str);
    g_free(arg_str);

    if (in_state == NULL)
    {
        rra_cli_result_add_error(ret_val, _("RRACliState is NULL"));
        return ret_val;
    }

    if (argc != 5)
    {
        rra_cli_result_add_error(
            ret_val,
            _("modify_value requires four arguments: "
              "<sub operation> <path> <value name> <byte number> <mask>"));
        return ret_val;
    }

    rra_path *test_path = massage_path(in_state, arg_path);

    ValueKeyCell *vkc =
        registry_path_get_value(rra_cli_state_get_registry(in_state),
                                rra_path_as_str(test_path), arg_val_name);

    if (vkc == NULL)
    {
        rra_cli_result_add_error(
            ret_val,
            _("No value at path %s"),
            rra_path_as_str(test_path));
    }
    else
    {
        struct BitRegData bdata;

        if(!str_to_guint32(arg_byte_num, &bdata.byte))
        {
            rra_cli_result_add_error(
                ret_val, _("Byte number (%s) isn't a 32 bit unsigned int"),
                arg_byte_num);
        }

        if (!str_to_guint8(arg_byte_mask, &bdata.mask))
        {
            rra_cli_result_add_error(
                ret_val, _("Mask (%s) isn't an 8 bit unsigned int"),
                arg_byte_mask);
        }

        if (rra_cli_result_is_error(ret_val))
        {
            /* Don't do anything else.  Got a conversion error. */
        }
        else if (strcmp("clear_bit", arg_op) == 0)
        {
            if (!value_key_cell_modify_val(vkc, clear_bit_vkcmodder, &bdata))
            {
                rra_cli_result_add_error(ret_val,
                                           _("clear bit modify failed"));
            }
        }
        else if (strcmp("set_bit", arg_op) == 0)
        {
            if (!value_key_cell_modify_val(vkc, set_bit_vkcmodder, &bdata))
            {
                rra_cli_result_add_error(ret_val,
                                           _("set bit modify failed"));
            }
        }
        else
        {
            rra_cli_result_add_error(
                ret_val,
                _("The only suboperations supported are "
                  "clear_bit and set_bit"));
        }
    }

    rra_cli_result_set_success(ret_val);
    return ret_val;
}
