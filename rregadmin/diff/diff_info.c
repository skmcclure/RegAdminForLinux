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

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/diff/diff_info.h>
#include <rregadmin/diff/log.h>
#include <rregadmin/util/malloc.h>

struct rra_diff_info_
{
    GPtrArray *action_list;
    int compare_value;
};

rra_diff_info*
rra_diff_info_new(void)
{
    rra_diff_info *ret_val = rra_new_type(rra_diff_info);
    ret_val->action_list = g_ptr_array_new();
    ret_val->compare_value = 0;
    return ret_val;
}

static gboolean
free_action(rra_diff_action *in_action)
{
    switch(in_action->action)
    {
    case RRA_DI_ADD_KEY:
    case RRA_DI_DELETE_KEY:
    case RRA_DI_HIVE_NAME_CHANGE:
        ustr_free((ustring*)in_action->data);
        break;

    case RRA_DI_ADD_VALUE:
    case RRA_DI_DELETE_VALUE:
        return rra_di_value_action_free(
            (rra_di_value_action*)in_action->data);
        break;

    case RRA_DI_VERSION_CHANGE:
        g_free(in_action->data);
        break;

    default:
        return FALSE;
    };

    in_action->data = NULL;
    in_action->action = RRA_DI_NULL;

    rra_free_type(rra_diff_action, in_action);

    return TRUE;
}

static gboolean
free_actions(GPtrArray *in_arr)
{
    gboolean ret_val = TRUE;
    while (in_arr->len > 0)
    {
        rra_diff_action *action =
            (rra_diff_action*)g_ptr_array_remove_index_fast(in_arr, 0);
        if (!free_action(action))
        {
            ret_val = FALSE;
        }
    }

    return ret_val;
}

gboolean
rra_diff_info_free(rra_diff_info *in_info)
{
    if (in_info == NULL)
    {
        return FALSE;
    }

    gboolean ret_val = TRUE;

    ret_val = free_actions(in_info->action_list);

    g_ptr_array_free(in_info->action_list, TRUE);
    in_info->action_list = NULL;

    rra_free_type(rra_diff_info, in_info);
    return ret_val;
}

gboolean
rra_di_set_compare_value(rra_diff_info *in_info, int in_val)
{
    if (in_info == NULL)
    {
        return FALSE;
    }

    if (in_info->compare_value == 0)
    {
        in_info->compare_value = in_val;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

gboolean
rra_di_clear_compare_value(rra_diff_info *in_info)
{
    if (in_info == NULL)
    {
        return FALSE;
    }

    in_info->compare_value = 0;
    return TRUE;
}

int
rra_di_get_compare_value(const rra_diff_info *in_info)
{
    if (in_info == NULL)
    {
        return 0;
    }

    return in_info->compare_value;
}

gchar *
rra_di_action_to_string(const rra_diff_action *in_action)
{
    switch(in_action->action)
    {
    case RRA_DI_ADD_KEY:
        return g_strdup_printf(_("ADD_KEY(%s)"),
                               ustr_as_utf8((ustring*)in_action->data));
        break;

    case RRA_DI_DELETE_KEY:
        return g_strdup_printf(_("DELETE_KEY(%s)"),
                               ustr_as_utf8((ustring*)in_action->data));
        break;

    case RRA_DI_ADD_VALUE:
        {
            rra_di_value_action *va = (rra_di_value_action*)in_action->data;
            ustring *val_str = value_get_as_string(va->val_new);
            gchar *ret_val = g_strdup_printf(
                _("ADD_VALUE(%s, %s, %s, %s)"),
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
                _("DELETE_VALUE(%s, %s, %s, %s)"),
                ustr_as_utf8(va->path), ustr_as_utf8(va->name),
                value_get_type_str(va->val_old), ustr_as_utf8(val_str));
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
                _("CHANGE_VALUE(%s, %s, %s, %s, %s, %s)"),
                ustr_as_utf8(va->path), ustr_as_utf8(va->name),
                value_get_type_str(va->val_old), ustr_as_utf8(val_old_str),
                value_get_type_str(va->val_new), ustr_as_utf8(val_new_str));
            ustr_free(val_old_str);
            ustr_free(val_new_str);
            return ret_val;
        }
        break;

    case RRA_DI_VERSION_CHANGE:
        return g_strdup_printf(_("HIVE_VERSION_CHANGE()"));
        break;

    case RRA_DI_HIVE_NAME_CHANGE:
        return g_strdup_printf(_("HIVE_NAME_CHANGE(%s)"),
                               ustr_as_utf8((ustring*)in_action->data));
        break;

    default:
        return g_strdup(_("Unknown action type"));
    };
}

rra_di_value_action*
rra_di_value_action_new(ustring *in_path, ustring *in_name,
                        Value *in_val_old, Value *in_val_new)
{
    rra_di_value_action *ret_val = rra_new_type(rra_di_value_action);
    if (ret_val == NULL)
    {
        return NULL;
    }

    ret_val->path = in_path;
    ret_val->name = in_name;
    ret_val->val_old = in_val_old;
    ret_val->val_new = in_val_new;

    return ret_val;
}

gboolean
rra_di_value_action_free(rra_di_value_action *in_action)
{
    ustr_free(in_action->path);
    ustr_free(in_action->name);
    if (in_action->val_old != NULL)
    {
        value_free(in_action->val_old);
    }
    if (in_action->val_new != NULL)
    {
        value_free(in_action->val_new);
    }

    rra_free_type(rra_di_value_action, in_action);
    return TRUE;
}

guint32
rra_di_action_count(const rra_diff_info *in_info)
{
    return in_info->action_list->len;
}

const rra_diff_action*
rra_di_get_action(const rra_diff_info *in_info, guint32 in_index)
{
    if (in_info == NULL)
    {
        return NULL;
    }

    if (in_index >= in_info->action_list->len)
    {
        return NULL;
    }

    return (rra_diff_action*)g_ptr_array_index(in_info->action_list, in_index);
}

static rra_diff_action*
make_action(DiffInfoAction in_action, void *in_data)
{
    if (in_data == NULL)
    {
        return NULL;
    }

    rra_diff_action *ret_val = rra_new_type(rra_diff_action);
    ret_val->action = in_action;
    ret_val->data = in_data;

    return ret_val;
}

static gboolean
add_action(rra_diff_info *in_info, rra_diff_action *in_action)
{
    if (in_info == NULL)
    {
        if (in_action != NULL)
        {
            free_action(in_action);
        }
        return FALSE;
    }

    if (in_action == NULL)
    {
        return FALSE;
    }

    gchar *msg = rra_di_action_to_string(in_action);
    rra_info(N_("Adding action: %s"), msg);
    g_free(msg);

    g_ptr_array_add(in_info->action_list, in_action);

    return TRUE;
}

gboolean
rra_di_add_key(rra_diff_info *in_info, ustring *in_path)
{
    if (in_info == NULL || in_path == NULL || ustr_strlen(in_path) == 0)
    {
        return FALSE;
    }

    return add_action(in_info, make_action(RRA_DI_ADD_KEY, in_path));
}

gboolean
rra_di_delete_key(rra_diff_info *in_info, ustring *in_path)
{
    if (in_info == NULL || in_path == NULL || ustr_strlen(in_path) == 0)
    {
        return FALSE;
    }

    return add_action(in_info, make_action(RRA_DI_DELETE_KEY, in_path));
}

gboolean
rra_di_add_value(rra_diff_info *in_info, ustring *in_path,
                 ustring *in_name, Value *in_val)
{
    if (in_info == NULL
        || in_path == NULL || ustr_strlen(in_path) == 0
        || in_name == NULL
        || in_val == NULL)
    {
        return FALSE;
    }

    rra_di_value_action* va =
        rra_di_value_action_new(in_path, in_name, NULL, in_val);
    if (va == NULL)
    {
        return FALSE;
    }

    return add_action(in_info, make_action(RRA_DI_ADD_VALUE, va));
}

gboolean
rra_di_delete_value(rra_diff_info *in_info, ustring *in_path,
                    ustring *in_name, Value *in_val)
{
    if (in_info == NULL
        || in_path == NULL || ustr_strlen(in_path) == 0
        || in_name == NULL
        || in_val == NULL)
    {
        return FALSE;
    }

    rra_di_value_action* va =
        rra_di_value_action_new(in_path, in_name, in_val, NULL);
    if (va == NULL)
    {
        return FALSE;
    }

    return add_action(in_info, make_action(RRA_DI_DELETE_VALUE, va));
}

gboolean
rra_di_change_value(rra_diff_info *in_info, ustring *in_path,
                    ustring *in_name,
                    Value *in_val_old, Value *in_val_new)
{
    if (in_info == NULL
        || in_path == NULL
        || ustr_strlen(in_path) == 0
        || in_name == NULL
        || in_val_old == NULL
        || in_val_new == NULL)
    {
        return FALSE;
    }

    rra_di_value_action* va =
        rra_di_value_action_new(in_path, in_name, in_val_old, in_val_new);
    if (va == NULL)
    {
        return FALSE;
    }

    return add_action(in_info, make_action(RRA_DI_CHANGE_VALUE, va));
}

gboolean
rra_di_change_version(rra_diff_info *in_info, HiveVersion *in_version)
{
    if (in_info == NULL || in_version == NULL)
    {
        return FALSE;
    }

    return add_action(in_info, make_action(RRA_DI_VERSION_CHANGE, in_version));
}

gboolean
rra_di_change_hive_name(rra_diff_info *in_info, ustring *in_name)
{
    if (in_info == NULL || in_name == NULL || ustr_strlen(in_name) == 0)
    {
        return FALSE;
    }

    return add_action(in_info, make_action(RRA_DI_HIVE_NAME_CHANGE, in_name));
}
