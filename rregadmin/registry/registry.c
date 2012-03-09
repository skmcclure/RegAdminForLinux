/*
 * Authors:     Sean Loaring
 *              Petter Nordahl-Hagen
 *              James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi Inc
 * Copyright (c) 2005-2006 Sean Loaring
 * Copyright (c) 1997-2004 Petter Nordahl-Hagen
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

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/registry/registry.h>
#include <rregadmin/registry/registry_key.h>
#include <rregadmin/registry/hive_info.h>
#include <rregadmin/registry/regkeygen.h>
#include <rregadmin/registry/search_order.h>
#include <rregadmin/hive/util.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/hive_check.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/registry/log.h>
#include <rregadmin/util/path.h>
#include <rregadmin/util/fs.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/util/giochan_helpers.h>
#include <rregadmin/util/malloc.h>

static const gboolean REGISTRY_DEBUG = FALSE;

static reg_id findid (const Registry *in_reg, const rra_path *check,
                      rra_path *subpath_dest);
static KeyCell* get_key_cell(const Registry *in_reg, const char *in_path,
                             gboolean in_create);
static ValueKeyCell* get_value_cell(const Registry *in_reg,
                                    const char *in_path,
                                    const char *in_value_name);

/** Structure containing registry data.
 *
 * @internal
 *
 * @ingroup registry_registry_group
 */
struct Registry_
{
    GPtrArray *hives;
    int hive_count;
    GTree *keys;
    reg_id last_key_id;

    gboolean dirty;
    guint32 options;
};

reg_id ID_HKEY_CLASSES_ROOT = 0;
reg_id ID_HKEY_CURRENT_USER = 1;
reg_id ID_HKEY_LOCAL_MACHINE = 2;
reg_id ID_HKEY_USERS = 3;
reg_id ID_HKEY_PERFORMANCE_DATA = 4;
reg_id ID_HKEY_CURRENT_CONFIG = 5;
reg_id ID_HKEY_DYN_DATA = 6;

static hive_info*
get_hi(const Registry *in_reg, hive_id in_id)
{
    if (in_reg == NULL
        || in_id < 0)
    {
        rra_warning(N_("registry (%p) or id(%d) invalid"), in_reg, in_id);
        return NULL;
    }

    if (in_id >= in_reg->hives->len)
    {
        return NULL;
    }
    else
    {
        hive_info *hi = (hive_info*)g_ptr_array_index(in_reg->hives, in_id);
        if (hive_info_is_valid(hi))
        {
            return hi;
        }
        else
        {
            return NULL;
        }
    }
}

static void
insert_hi(Registry *in_reg, hive_info *in_hi)
{
    g_ptr_array_add(in_reg->hives, (gpointer)in_hi);
    hive_info_set_id(in_hi, in_reg->hives->len - 1);
    in_reg->hive_count++;
    rra_debug("Inserting hive_info(%p) with id %d",
              in_hi, hive_info_get_id(in_hi));
}

static gboolean
remove_hi(Registry *in_reg, hive_id in_id)
{
    hive_info *hi = get_hi(in_reg, in_id);
    if (hi == NULL)
    {
        return FALSE;
    }

    rra_debug("Removing hive_info with id %d", in_id);
    if (hive_info_unload(hi, FALSE))
    {
        in_reg->hive_count--;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static RegistryKey*
get_rk(const Registry *in_reg, reg_id in_id)
{
    if (in_reg == NULL || in_id < 0)
    {
        rra_warning(N_("registry (%p) or id(%d) invalid"), in_reg, in_id);
        return NULL;
    }

    return (RegistryKey*)g_tree_lookup(in_reg->keys, GINT_TO_POINTER(in_id));
}

static void
insert_rk(Registry *in_reg, RegistryKey *in_key)
{
    g_tree_insert(in_reg->keys,
                  GINT_TO_POINTER(registry_key_get_id(in_key)),
                  in_key);
    rra_debug("Inserting key %d with path %s",
              registry_key_get_id(in_key),
              rra_path_as_str(registry_key_get_path(in_key)));

    if (registry_key_get_id(in_key) > in_reg->last_key_id)
    {
        in_reg->last_key_id = registry_key_get_id(in_key);
    }
}

static void
populate_default_meta (Registry *in_reg)
{
#define insert_one(id, path)                            \
    insert_rk(in_reg,                                   \
              registry_key_new_meta(in_reg, path, id))

    insert_one(ID_HKEY_CLASSES_ROOT, "\\HKEY_CLASSES_ROOT");
    insert_one(ID_HKEY_CURRENT_USER, "\\HKEY_CURRENT_USER");
    insert_one(ID_HKEY_LOCAL_MACHINE, "\\HKEY_LOCAL_MACHINE");
    insert_one(ID_HKEY_USERS, "\\HKEY_USERS");
    insert_one(ID_HKEY_PERFORMANCE_DATA, "\\HKEY_PERFORMANCE_DATA");
    insert_one(ID_HKEY_CURRENT_CONFIG, "\\HKEY_CURRENT_CONFIG");
    insert_one(ID_HKEY_DYN_DATA, "\\HKEY_DYN_DATA");
#undef insert_one
}

static gint
compare_ints(gconstpointer a, gconstpointer b,
             gpointer DECLARE_UNUSED(data))
{
    return GPOINTER_TO_INT(b) - GPOINTER_TO_INT(a);
}

static void
destroy_reg_key(gpointer p)
{
    if (p != NULL)
        registry_key_free((RegistryKey*)p);
}

Registry*
registry_new(void)
{
    Registry *ret_val = rra_new_type(Registry);

    ret_val->hives = g_ptr_array_new();
    ret_val->hive_count = 0;

    ret_val->keys = g_tree_new_full(compare_ints, 0, 0, destroy_reg_key);

    ret_val->dirty = FALSE;
    ret_val->options = 0;
    ret_val->last_key_id = -1;

    populate_default_meta(ret_val);

    return ret_val;
}

Registry*
registry_new_win_dir(const char *in_dir)
{
    Registry *ret_val = registry_new();

    if (registry_load_win_dir(ret_val, in_dir))
    {
        return ret_val;
    }
    else
    {
        rra_warning(N_("registry_load_win_dir failed"));
        registry_free(ret_val);
        return NULL;
    }
}

gboolean
registry_free(Registry *in_reg)
{
    if (in_reg == NULL)
    {
        return FALSE;
    }

    if (in_reg->hives)
    {
        while (in_reg->hives->len > 0)
        {
            hive_info *hi =
                (hive_info*)g_ptr_array_remove_index_fast(in_reg->hives, 0);
            hive_info_free(hi);
        }
        g_ptr_array_free(in_reg->hives, TRUE);
        in_reg->hives = NULL;
    }

    if (in_reg->keys)
    {
        g_tree_destroy(in_reg->keys);
        in_reg->keys = NULL;
    }

    rra_free_type(Registry, in_reg);

    return TRUE;
}

static const char* hive_files[] = {
    "system32/config/systemprofile/ntuser.dat",
    "system32/config/default",
    "system32/config/sam",
    "system32/config/security",
    "system32/config/software",
    "system32/config/system",
    "system32/config/userdiff"
};

gboolean
registry_load_win_dir(Registry *in_reg, const char *in_dir)
{
    ustring *dir_name = ustr_create(in_dir);
    if (!filesysob_exists(dir_name))
    {
        rra_warning(N_("No directory %s"), in_dir);
        return FALSE;
    }

    guint i;
    for (i = 0; i < G_N_ELEMENTS(hive_files); i++)
    {
        ustring *file_name = ustr_copy(dir_name);

        ustr_strcat(file_name, "/");
        ustr_strcat(file_name, hive_files[i]);

        rra_debug(N_("Trying to load hive %s"), ustr_as_utf8(file_name));

        if (find_file_casei_bang(file_name))
        {
            rra_debug(N_("Found hive %s"), ustr_as_utf8(file_name));

            if (filesysob_exists_and_non_zero_sized(file_name))
            {
                rra_debug(N_("Loading hive %s"), ustr_as_utf8(file_name));

                if (registry_load_hive(in_reg, ustr_as_utf8(file_name))
                    == -1)
                {
                    rra_warning(N_("Problem loading %s"),
                                ustr_as_utf8(file_name));
                }
            }
        }
        else
        {
            /*
             * Comment out since it's ok if the file doesn't exist.
             */
            rra_message("Hive %s doesn't exist", ustr_as_utf8(file_name));
        }

        ustr_free(file_name);
    }

    ustr_free(dir_name);

    return TRUE;
}

gboolean
registry_set_option(Registry *in_reg, RegistryOption in_opt)
{
    if (in_reg == NULL)
    {
        return FALSE;
    }

    in_reg->options |= in_opt;

    return TRUE;
}

gboolean
registry_unset_option(Registry *in_reg, RegistryOption in_opt)
{
    if (in_reg == NULL)
    {
        return FALSE;
    }

    in_reg->options &= ~in_opt;

    return TRUE;
}

gboolean
registry_has_option(const Registry *in_reg, RegistryOption in_opt)
{
    if (in_reg == NULL)
    {
        return FALSE;
    }

    return (in_reg->options & in_opt) > 0;
}

gint
registry_number_of_hives(const Registry *in_reg)
{
    if (in_reg == NULL)
    {
        return -1;
    }

    return in_reg->hive_count;
}

const RegistryKey*
registry_get_registry_key(const Registry *in_reg, gint in_id)
{
    if (in_reg == NULL
        || in_id < 0)
    {
        return NULL;
    }

    return (const RegistryKey*)g_tree_lookup(in_reg->keys,
                                             GINT_TO_POINTER(in_id));
}

const RegistryKey*
registry_get_base_registry_key(const Registry *in_reg, hive_id in_id)
{
    gint i;

    for (i = registry_key_id_start(); i <= in_reg->last_key_id; i++)
    {
        RegistryKey *key = get_rk(in_reg, i);

        if (key != NULL)
        {
            if (registry_key_get_hive_id(key) == in_id
                && registry_key_get_type(key) == REG_KEY_TYPE_BASE)
            {
                return key;
            }
        }
    }

    return NULL;
}

const RegistryKey*
registry_find_registry_key(const Registry *in_reg, const char *in_path)
{
    const RegistryKey *ret_val;

    rra_path *path = rra_path_new_win(in_path);

    reg_id id = findid(in_reg, path, 0);
    if (id != -1)
    {
        ret_val = registry_get_registry_key(in_reg, id);
    }
    else
    {
        ret_val = NULL;
    }

    rra_path_free(path);
    return ret_val;
}

gboolean
registry_is_dirty(const Registry *in_reg)
{
    if (in_reg == NULL)
    {
        return FALSE;
    }

    if (in_reg->dirty)
    {
        return TRUE;
    }

    int i;
    for (i = 0; i < in_reg->hives->len; i++)
    {
        hive_info *hi = get_hi(in_reg, i);
        if (hi != NULL)
        {
            if (hive_is_dirty(hive_info_get_hive(hi)))
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}

gboolean
registry_set_dirty(Registry *in_reg, gboolean in_value)
{
    gboolean ret_val = registry_is_dirty(in_reg);

    in_reg->dirty = in_value;

    return ret_val;
}

gboolean
registry_flush(Registry *in_reg, hive_id in_id)
{
    hive_info *hi = get_hi(in_reg, in_id);
    if (hi != NULL
        && hive_info_is_dirty(hi)
        && registry_has_option(in_reg, REG_OPT_CHECK_HIVES_WRITE))
    {
        if (!hive_check(hive_info_get_hive(hi), RRA_CHECK_VERBOSITY_QUIET))
        {
            rra_warning(N_("Hive didn't check out: %s"),
                        hive_info_get_filename(hi));
            return FALSE;
        }
    }

    return hive_info_flush(hi, registry_has_option(in_reg, REG_OPT_BACKUP));
}

gboolean
registry_flush_all(Registry *in_reg)
{
    int i;
    gboolean ret_val = TRUE;

    if (in_reg == NULL)
    {
        return FALSE;
    }

    for (i = 0; i < in_reg->hives->len; i++)
    {
        // Only remove the existing ones.  ignore ones that might have
        // been previously removed.
        if (get_hi(in_reg, i) != NULL)
        {
            if (!registry_flush(in_reg, i))
            {
                ret_val = FALSE;
            }
        }
    }

    in_reg->dirty = FALSE;

    return ret_val;
}

hive_id
registry_load_hive(Registry *in_reg, const char *in_filename)
{
    return registry_load_hive_full(in_reg, in_filename, NULL);
}

hive_id
registry_load_hive_full(Registry *in_reg, const char *in_filename,
                        const char *in_mount_point)
{
    guint i;
    hive_info *hi = hive_info_new(in_filename);

    if (hi == NULL)
    {
        rra_warning(N_("Unable to read hive from %s"), in_filename);
        return -1;
    }

    if (registry_has_option(in_reg, REG_OPT_CHECK_HIVES_READ))
    {
        if (!hive_check(hive_info_get_hive(hi), RRA_CHECK_VERBOSITY_QUIET))
        {
            rra_warning(N_("Hive didn't check out: %s"), in_filename);
        }
    }

    insert_hi(in_reg, hi);

    if (in_mount_point == NULL)
    {
        GPtrArray *keys = reg_key_gen(in_reg, hive_info_get_id(hi));

        if (keys != NULL)
        {
            for (i = 0; i < keys->len; i++)
            {
                insert_rk(in_reg, (RegistryKey*)g_ptr_array_index(keys, i));
            }

            g_ptr_array_free(keys, TRUE);
        }
    }
    else
    {
        RegistryKey *key = registry_key_new_base(in_reg,
                                                 hive_info_get_id(hi),
                                                 in_mount_point);
        if (key != NULL)
        {
            insert_rk(in_reg, key);
        }
    }

    return hive_info_get_id(hi);
}

gboolean
registry_unload(Registry *in_reg, hive_id in_id)
{
    if (in_reg == NULL)
    {
        return FALSE;
    }

    return remove_hi(in_reg, in_id);
}

gboolean
registry_unload_all_force(Registry *in_reg)
{
    int i;
    gboolean ret_val = TRUE;

    if (in_reg == NULL)
    {
        return FALSE;
    }

    for (i = 0; i < in_reg->hives->len; i++)
    {
        // Only remove the existing ones.  ignore ones that might have
        // been previously removed.
        if (get_hi(in_reg, i) != NULL)
        {
            if (!registry_unload(in_reg, i))
            {
                ret_val = FALSE;
            }
        }
    }

    return ret_val;
}

gboolean
registry_unload_all(Registry *in_reg)
{
    if (registry_is_dirty(in_reg))
    {
        return FALSE;
    }

    return registry_unload_all_force(in_reg);
}

gboolean
registry_contains_hivetype(const Registry *in_reg, HiveType in_type)
{
    int i;

    if (in_reg == NULL)
    {
        return FALSE;
    }

    for (i = 0; i < in_reg->hives->len; i++)
    {
        hive_info *hi = get_hi(in_reg, i);

        if (hi != NULL)
        {
            if (hive_info_get_type(hi) == in_type)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}

const hive_info*
registry_get_hive_info(const Registry *in_reg, hive_id in_id)
{
    return get_hi(in_reg, in_id);
}

Hive*
registry_id_get_hive(const Registry *in_reg, hive_id in_id)
{
    hive_info *hi = get_hi(in_reg, in_id);
    if (hi != NULL)
        return hive_info_get_hive(hi);
    else
        return NULL;
}

int
registry_id_get_hive_type(const Registry *in_reg, hive_id in_id)
{
    hive_info *hi = get_hi(in_reg, in_id);
    if (hi != NULL)
        return hive_info_get_type(hi);
    else
        return HTYPE_UNKNOWN;
}

const char*
registry_id_get_hive_filename(const Registry *in_reg, hive_id in_id)
{
    hive_info *hi = get_hi(in_reg, in_id);
    if (hi != NULL)
        return hive_info_get_filename(hi);
    else
        return "";
}

gboolean
registry_path_is_key(const Registry *in_reg, const char *in_path)
{
    if (in_reg == NULL || in_path == NULL)
    {
        return FALSE;
    }

    KeyCell *c = get_key_cell(in_reg, in_path, FALSE);

    return (c != NULL);
}

gboolean
registry_path_is_value(const Registry *in_reg, const char *in_path,
                       const char *in_value_name)
{
    if (in_reg == NULL || in_path == NULL || in_value_name == NULL)
    {
        return FALSE;
    }

    ValueKeyCell *c = get_value_cell(in_reg, in_path, in_value_name);

    return (c != NULL);
}

KeyCell*
registry_path_get_key(const Registry *in_reg, const char *in_path)
{
    if (in_reg == NULL || in_path == NULL)
    {
        return NULL;
    }

    KeyCell *c = get_key_cell(in_reg, in_path, FALSE);

    return c;
}

KeyCell*
registry_path_add_key(const Registry *in_reg, const char *in_path)
{
    if (in_reg == NULL || in_path == NULL)
    {
        return NULL;
    }

    KeyCell *c = get_key_cell(in_reg, in_path, TRUE);

    return c;
}

gboolean
registry_path_is_deletable_key(const Registry *in_reg, const char *in_path)
{
    rra_path *path = rra_path_new_win(in_path);
    rra_path_normalize(path);

    reg_id id = findid(in_reg, path, NULL);

    if (id == -1)
    {
        rra_message(N_("Didn't find registry key"));
        return FALSE;
    }

    const RegistryKey *key = registry_get_registry_key(in_reg, id);

    const rra_path *reg_path = registry_key_get_path(key);

    if (rra_path_equal(path, reg_path))
    {
        rra_message(N_("Paths equal: '%s' vs '%s'"),
                    rra_path_as_str(path),
                    rra_path_as_str(reg_path));
        return FALSE;
    }
    else
    {
        rra_message(N_("Paths not equal: '%s' vs '%s'"),
                    rra_path_as_str(path),
                    rra_path_as_str(reg_path));
        return TRUE;
    }
}

gboolean
registry_path_delete_key(const Registry *in_reg, const char *in_path)
{
    gboolean ret_val = FALSE;
    if (in_reg == NULL || in_path == NULL)
    {
        return FALSE;
    }

    KeyCell *c = get_key_cell(in_reg, in_path, FALSE);

    if (c == NULL)
    {
        rra_warning(N_("Didn't find key %s"), in_path);
        return FALSE;
    }

    KeyCell *parent = key_cell_get_parent(c);

    if (parent == NULL)
    {
        rra_warning(N_("Couldn't get parent for key %s"), in_path);
        return FALSE;
    }

    ustring *c_name = key_cell_get_name(c);
    ret_val = key_cell_delete_subkey_str(parent, ustr_as_utf8(c_name));
    ustr_free(c_name);

    if (!ret_val)
    {
        rra_warning(N_("key_cell_delete_subkey_str failed for key %s"),
                    in_path);
    }

    return ret_val;
}

ValueKeyCell*
registry_path_get_value(const Registry *in_reg, const char *in_path,
                        const char *in_value_name)
{
    if (in_reg == NULL || in_path == NULL || in_value_name == NULL)
    {
        return NULL;
    }

    ValueKeyCell *c = get_value_cell(in_reg, in_path, in_value_name);

    return c;
}

ValueKeyCell*
registry_path_add_value(const Registry *in_reg,
                        const char *in_path,
                        const char *in_value_name,
                        const Value *in_val)
{
    if (in_reg == NULL || in_path == NULL || in_val == NULL
        || in_value_name == NULL)
    {
        return NULL;
    }

    ValueKeyCell *ret_val = NULL;

    rra_path *path = rra_path_new_win(in_path);
    char *new_value_key;

    new_value_key = g_strdup(in_value_name);

    KeyCell *kc = get_key_cell(in_reg, rra_path_as_str(path), TRUE);
    rra_path_free(path);

    if (kc != NULL)
    {
        ValueKeyCell *sub_vkc;
        sub_vkc = key_cell_get_value_str(kc, new_value_key);

        if (sub_vkc != NULL)
        {
            rra_warning(N_("trying to add already existing value %s"),
                        new_value_key);
        }
        else
        {
            ret_val = key_cell_add_value(kc, new_value_key, in_val);
        }
    }

    g_free(new_value_key);

    return ret_val;
}

gboolean
registry_path_delete_value(const Registry *in_reg, const char *in_path,
                           const char *in_value_name)
{
    gboolean ret_val = FALSE;

    if (in_reg == NULL || in_path == NULL || in_value_name == NULL)
    {
        return FALSE;
    }

    rra_path *path = rra_path_new_win(in_path);
    char *new_value_key;

    new_value_key = g_strdup(in_value_name);

    KeyCell *kc = get_key_cell(in_reg, rra_path_as_str(path), FALSE);
    rra_path_free(path);

    if (kc != NULL)
    {
        ret_val = key_cell_delete_value_str (kc, new_value_key);
    }

    g_free(new_value_key);

    return ret_val;
}

static reg_id
findid (const Registry *in_reg, const rra_path *check,
        rra_path *subpath_dest)
{
    int i;
    rra_path *subpath = NULL;
    int best = -1;

    rra_info(N_("Looking for key with path %s"), rra_path_as_str(check));

    for (i = registry_key_id_start(); i <= in_reg->last_key_id; i++)
    {
        const RegistryKey *rk = registry_get_registry_key(in_reg, i);

        if (rk != NULL)
        {
            rra_path *tmp_path =
                rra_path_make_subpath(registry_key_get_path(rk), check);
            rra_info(N_("Looking at key id %d: %s"),
                     i, rra_path_as_str(registry_key_get_path(rk)));

            if (tmp_path != NULL)
            {
                rra_debug("tmp_path = %s", rra_path_as_str(tmp_path));

                if (subpath == NULL)
                {
                    subpath = rra_path_new_win("");
                    rra_path_set(subpath, tmp_path);
                    best = i;
                }
                else if (rra_path_number_of_parts(tmp_path)
                         <= rra_path_number_of_parts(subpath))
                {
                    rra_path_set(subpath, tmp_path);
                    best = i;
                }

                rra_path_free(tmp_path);
            }
        }
        else
        {
            rra_info(N_("No key at %d"), i);
        }
    }

    if (subpath_dest != NULL && subpath != NULL)
    {
        rra_path_set(subpath_dest, subpath);
    }

    rra_path_free(subpath);

    return best;
}

static KeyCell*
get_key_cell(const Registry *in_reg, const char *in_path,
             gboolean in_create)
{
    KeyCell *tnk;

    rra_path *path = rra_path_new_win(in_path);
    rra_path_normalize(path);

    reg_id id = findid(in_reg, path, path);

    if (id == -1)
    {
        rra_warning(N_("Didn't find hive for path %s"),
                    rra_path_as_str(path));
        return NULL;
    }

    const RegistryKey *key = registry_get_registry_key(in_reg, id);

    tnk = registry_key_get_key_cell(key);

    tnk = kc_trav_path_internal(tnk, path, in_create);

    rra_path_free(path);
    return tnk;
}

static ValueKeyCell*
get_value_cell(const Registry *in_reg, const char *in_path,
               const char *in_value_name)
{
    ValueKeyCell *ret_val = NULL;

    KeyCell *kc = get_key_cell(in_reg, in_path, FALSE);

    if (kc != NULL)
    {
        ret_val = key_cell_get_value_str (kc, in_value_name);
    }

    return ret_val;
}

gboolean
registry_get_xml_output(const Registry *in_reg, ustring *in_output)
{
    int i;

    ustr_printfa (in_output, "<Registry>\n");

    ustr_printfa (in_output, "    <Hives count=\"%d\">\n",
                  in_reg->hives->len);

    for (i = 0; i < in_reg->hives->len; i++)
    {
        hive_info *hi = get_hi(in_reg, i);
        hive_get_xml_output(hive_info_get_hive(hi), in_output, FALSE);
    }

    ustr_printfa (in_output, "    </Hives>\n");
    ustr_printfa (in_output, "</Registry>\n");

    return TRUE;
}
