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

#include <string.h>

#include <glib.h>

#include <rregadmin/registry/registry_key.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/util/path.h>

#define MAGIC_REAL_KEY_START 200

static guint32 next_reg_id = MAGIC_REAL_KEY_START;

/** Structure containing registry key data.
 *
 * @internal
 *
 * @ingroup registry_key_registry_group
 */
struct RegistryKey_
{
    reg_id id;
    Registry *reg;
    hive_id hive;

    RegistryKeyType type;
    rra_path *path;
    KeyCell *kcell;
    gboolean main;
};

reg_id
registry_key_id_start(void)
{
    return MAGIC_REAL_KEY_START;
}

static RegistryKey*
registry_key_new (Registry *in_reg, const char *in_absolute_path)
{
    if (in_reg == NULL
        || in_absolute_path == NULL)
    {
        return NULL;
    }

    rra_path *my_path = rra_path_new_win(in_absolute_path);

    if (rra_path_is_relative(my_path))
    {
        rra_path_free(my_path);
        return NULL;
    }

    RegistryKey* ret_val;

    ret_val = g_new0 (RegistryKey, 1);

    ret_val->id = -1;
    ret_val->reg = in_reg;
    ret_val->hive = -1;

    ret_val->type = -1;
    ret_val->path = my_path;
    ret_val->kcell = NULL;
    ret_val->main = TRUE;

    return ret_val;
}

RegistryKey*
registry_key_new_base(Registry *in_reg, hive_id in_id,
                      const char *in_absolute_path)
{
    RegistryKey *ret_val = registry_key_new(in_reg, in_absolute_path);

    if (ret_val != NULL)
    {
        ret_val->hive = in_id;
        if (in_id < 0)
        {
            registry_key_free(ret_val);
            return NULL;
        }

        ret_val->type = REG_KEY_TYPE_BASE;
        guint id = next_reg_id++;
        ret_val->id = id;
    }

    return ret_val;
}

RegistryKey*
registry_key_new_meta(Registry *in_reg, const char *in_absolute_path,
                      reg_id in_id)
{
    if (in_id >= MAGIC_REAL_KEY_START)
    {
        return NULL;
    }

    RegistryKey *ret_val = registry_key_new(in_reg, in_absolute_path);

    if (ret_val != NULL)
    {
        ret_val->type = REG_KEY_TYPE_META;
        ret_val->id = in_id;
    }

    return ret_val;
}

RegistryKey*
registry_key_new_link(Registry *in_reg, hive_id in_id,
                      const char *in_absolute_path, KeyCell *in_kc)
{
    if (in_kc == NULL)
    {
        return NULL;
    }

    RegistryKey *ret_val = registry_key_new_base(in_reg, in_id,
                                                 in_absolute_path);

    if (ret_val != NULL)
    {
        ret_val->type = REG_KEY_TYPE_LINK;
        ret_val->kcell = in_kc;
    }

    return ret_val;
}

RegistryKey*
registry_key_copy(const RegistryKey *in_key)
{
    if (in_key == NULL)
    {
        return NULL;
    }

    RegistryKey *ret_val = g_new0 (RegistryKey, 1);

    ret_val->id = in_key->id;
    ret_val->reg = in_key->reg;
    ret_val->hive = in_key->hive;

    ret_val->type = in_key->type;
    ret_val->path = rra_path_copy(in_key->path);
    ret_val->kcell = in_key->kcell;
    ret_val->main = FALSE;

    return ret_val;
}

gboolean
registry_key_free (const RegistryKey *in_key)
{
    if (in_key == NULL)
        return FALSE;

    if (in_key->main)
        return FALSE;

    if (in_key->path != NULL)
    {
        rra_path_free(in_key->path);
        ((RegistryKey*)in_key)->path = NULL;
    }

    g_free((RegistryKey*)in_key);

    return TRUE;
}

gboolean
registry_key_free_force (RegistryKey *in_key)
{
    if (in_key == NULL)
        return FALSE;

    if (in_key->path != NULL)
    {
        rra_path_free(in_key->path);
        in_key->path = NULL;
    }

    g_free(in_key);

    return TRUE;
}

reg_id
registry_key_get_id(const RegistryKey *in_key)
{
    return in_key->id;
}

const rra_path*
registry_key_get_path(const RegistryKey *in_key)
{
    return in_key->path;
}

Hive*
registry_key_get_hive(const RegistryKey *in_key)
{
    return registry_id_get_hive(in_key->reg, in_key->hive);
}

int
registry_key_get_hive_type(const RegistryKey *in_key)
{
    return registry_id_get_hive_type(in_key->reg, in_key->hive);
}

const char*
registry_key_get_hive_filename(const RegistryKey *in_key)
{
    return registry_id_get_hive_filename(in_key->reg, in_key->hive);
}

hive_id
registry_key_get_hive_id(const RegistryKey *in_key)
{
    return in_key->hive;
}

RegistryKeyType
registry_key_get_type(const RegistryKey *in_key)
{
    return in_key->type;
}

KeyCell*
registry_key_get_key_cell(const RegistryKey *in_key)
{
    if (in_key->kcell != NULL)
    {
        return in_key->kcell;
    }
    else
    {
        Hive *hv = registry_key_get_hive(in_key);
        if (hv != NULL)
            return hive_get_root_key(hv);
        else
            return NULL;
    }
}

gboolean
registry_key_set_key_cell(RegistryKey *in_key, KeyCell *in_cell)
{
    if (in_cell == NULL)
    {
        return FALSE;
    }

    // Don't allow mod of main key
    if (in_key->main)
    {
        return FALSE;
    }

    in_key->kcell = in_cell;
    return TRUE;
}
