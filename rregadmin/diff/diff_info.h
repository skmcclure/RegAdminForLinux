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

#ifndef RREGADMIN_DIFF_INFO_H
#define RREGADMIN_DIFF_INFO_H 1

#include <glib.h>

#include <rregadmin/util/ustring.h>
#include <rregadmin/util/value.h>
#include <rregadmin/hive/types.h>

G_BEGIN_DECLS

typedef struct rra_diff_info_ rra_diff_info;

typedef enum
{
    RRA_DI_NULL,
    RRA_DI_ADD_KEY,
    RRA_DI_DELETE_KEY,
    RRA_DI_ADD_VALUE,
    RRA_DI_CHANGE_VALUE,
    RRA_DI_DELETE_VALUE,
    RRA_DI_VERSION_CHANGE,
    RRA_DI_HIVE_NAME_CHANGE,

} DiffInfoAction;

struct rra_diff_action_
{
    DiffInfoAction action;
    void *data;
};
typedef struct rra_diff_action_ rra_diff_action;

struct rra_di_value_action_
{
    ustring *path;
    ustring *name;
    Value *val_old;
    Value *val_new;
};
typedef struct rra_di_value_action_ rra_di_value_action;

rra_diff_info* rra_diff_info_new(void);

gboolean rra_diff_info_free(rra_diff_info *in_info);

gboolean rra_di_set_compare_value(rra_diff_info *in_info, int in_val);

gboolean rra_di_clear_compare_value(rra_diff_info *in_info);

int rra_di_get_compare_value(const rra_diff_info *in_info);

rra_di_value_action* rra_di_value_action_new(ustring *in_path,
                                             ustring *in_name,
                                             Value *in_val_old,
                                             Value *in_val_new);

gboolean rra_di_value_action_free(rra_di_value_action *in_action);

gchar* rra_di_action_to_string(const rra_diff_action *in_action);

guint32 rra_di_action_count(const rra_diff_info *in_info);

const rra_diff_action* rra_di_get_action(const rra_diff_info *in_info,
                                         guint32 in_index);

gboolean rra_di_add_key(rra_diff_info *in_info,
                        ustring *in_path);

gboolean rra_di_delete_key(rra_diff_info *in_info,
                           ustring *in_path);

gboolean rra_di_add_value(rra_diff_info *in_info,
                          ustring *in_path, ustring *in_name,
                          Value *in_val);

gboolean rra_di_delete_value(rra_diff_info *in_info,
                             ustring *in_path, ustring *in_name,
                             Value *in_val);

gboolean rra_di_change_value(rra_diff_info *in_info,
                             ustring *in_path, ustring *in_name,
                             Value *in_val_old, Value *in_val_new);

gboolean rra_di_change_version(rra_diff_info *in_info,
                               HiveVersion *in_version);

gboolean rra_di_change_hive_name(rra_diff_info *in_info,
                                 ustring *in_name);

G_END_DECLS

#endif // RREGADMIN_DIFF_INFO_H
