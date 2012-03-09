/*
 * Authors:       James LewisMoss <jlm@racemi.com>
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

#include <rregadmin/registry/regkeygen.h>
#include <rregadmin/registry/registry_key.h>
#include <rregadmin/registry/consts.h>
#include <rregadmin/hive/util.h>
#include <rregadmin/registry/log.h>
#include <rregadmin/util/value.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/hive/key_cell.h>

static const char* get_base_mount_point(HiveType in_type);

static dword_type
get_dword_value(const hive_info *in_hi, const char *path,
                const char *in_value_name)
{
    ValueKeyCell *vkc = value_key_cell_trav_path(
        hive_info_get_hive(in_hi), NULL, path, in_value_name);

    if (vkc == NULL)
    {
        return -1;
    }
    else
    {
        Value *val = value_key_cell_get_val(vkc);
        if (val != NULL)
        {
            dword_type ret_val = value_get_as_dword(val);
            value_free(val);
            return ret_val;
        }
        else
        {
            return -1;
        }
    }
}

static KeyCell*
get_cs_kc(const hive_info *in_hi, const char *in_path,
          const char *in_value_name)
{
    dword_type control_set = get_dword_value(in_hi, in_path, in_value_name);
    ustring *path = ustr_new();
    ustr_printf(path,
                "ControlSet%03d",
                control_set);
    KeyCell *ret_val =  key_cell_trav_path(hive_info_get_hive(in_hi),
                                           0, ustr_as_utf8(path));
    ustr_free(path);
    return ret_val;
}

GPtrArray*
reg_key_gen(Registry* in_reg, hive_id in_id)
{
    if (in_reg == NULL)
    {
        return NULL;
    }

    if (in_id < 0)
    {
        return NULL;
    }

    GPtrArray *ret_val;
    const hive_info *hi = registry_get_hive_info(in_reg, in_id);
    if (hi == NULL)
    {
        return NULL;
    }

    HiveType type = hive_info_get_type(hi);

    if (type != HTYPE_ERROR)
    {
        const char *base_pt = get_base_mount_point(type);
        RegistryKey *key = registry_key_new_base(in_reg, in_id, base_pt);

        if (key == NULL)
        {
            rra_warning(
                _("Couldn't create RegistryKey for hive: %s "
                  "(mount point = %s; type = %d)"),
                hive_info_get_filename(hi),
                base_pt ? base_pt : _("(NULL)"),
                type);
            return NULL;
        }

        ret_val = g_ptr_array_new();
        g_ptr_array_add(ret_val, key);

        if (type == HTYPE_SYSTEM)
        {
            /* Link CurrentControlSet to appropriate ControlSet### */
            KeyCell *kc = get_cs_kc(hi, "Select", "Current");

            if (kc == NULL)
            {
                kc = get_cs_kc(hi, "Select", "Default");
            }

            if (kc == NULL)
            {
                rra_warning(
                    _("Couldn't find link location for CurrentControlSet"));
            }
            else
            {
                RegistryKey *cs_key = registry_key_new_link(
                    in_reg, in_id,
                    "\\HKEY_LOCAL_MACHINE\\system\\CurrentControlSet",
                    kc);
                g_ptr_array_add(ret_val, cs_key);
            }
        }
        else if (type == HTYPE_SOFTWARE)
        {
            //! \todo need to link HKEY_CLASSES_ROOT to
            //! HKEY_LOCAL_MACHINE/software/Classes
        }
    }
    else
    {
        ret_val = NULL;
    }

    return ret_val;
}

/* These MUST be in the same order as HTYPE_*, they are accessed
   thusly: hivepath[ HTYPE_TYPE ] */
static const char *hivepath[] = {
    UNKNOWN_KEY_NAME,
    SAM_KEY_NAME,
    SYSTEM_KEY_NAME,
    SECURITY_KEY_NAME,
    SOFTWARE_KEY_NAME,
    CURRENT_USER_KEY_NAME,
    USERDIFF_KEY_NAME,
    SID_RID_CLASSES_USER_KEY_NAME,
    DEFAULT_USER_KEY_NAME
};

static const char*
get_base_mount_point(HiveType in_type)
{
    int max = sizeof(hivepath) / sizeof(char*);

    if (in_type > 0 && in_type < max)
    {
        return hivepath[in_type];
    }
    else
    {
        return NULL;
    }
}
