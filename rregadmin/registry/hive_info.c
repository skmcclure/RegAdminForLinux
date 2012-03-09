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

#include <string.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#ifdef HAVE_GLIB_GSTDIO_H
#include <glib/gstdio.h>
#else
#include <stdio.h>
#define g_rename rename
#endif

#include <rregadmin/registry/hive_info.h>
#include <rregadmin/registry/registry_key.h>
#include <rregadmin/hive/util.h>
#include <rregadmin/registry/log.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/util/path.h>

/** Structure containing data about a loaded hive.
 *
 * @internal
 *
 * @ingroup hive_info_registry_group
 */
struct hive_info_
{
    int id;
    Hive *hive;
    HiveType type;
    char *filename;
};

hive_info*
hive_info_new (const char *in_filename)
{
    hive_info* result;
    result = g_new0 (hive_info, 1);
    result->id = -1;
    rra_info(N_("Trying to read hive %s"), in_filename);
    result->hive = hive_read(in_filename);
    if (result->hive == NULL)
    {
        g_free(result);
        return NULL;
    }

    result->filename = g_strdup(in_filename);
    result->type = guess_hive_type(result->hive);

    return result;
}

static gboolean
free_parts(hive_info *in_hi)
{
    if (in_hi == NULL)
    {
        return FALSE;
    }

    if (in_hi->hive != NULL)
    {
        hive_free(in_hi->hive);
        in_hi->hive = NULL;
    }

    if (in_hi->filename != NULL)
    {
        g_free(in_hi->filename);
        in_hi->filename = NULL;
    }

    return TRUE;
}

gboolean
hive_info_free (hive_info *in_hi)
{
    gboolean ret_val = free_parts(in_hi);
    g_free (in_hi);
    return ret_val;
}

static void
backup_hive(hive_info *in_hi)
{
    gchar *bkup_name =
        g_strdup_printf("%s.hbk", hive_info_get_filename(in_hi));

    g_rename(hive_info_get_filename(in_hi), bkup_name);

    g_free(bkup_name);
}

gboolean
hive_info_flush(hive_info *in_hi, gboolean in_do_backup)
{
    if (hive_info_is_dirty(in_hi))
    {
        if (in_do_backup)
        {
            backup_hive(in_hi);
        }

        return hive_write(hive_info_get_hive(in_hi),
                          hive_info_get_filename(in_hi));
    }

    return TRUE;
}

gboolean
hive_info_unload(hive_info *in_hi, gboolean in_do_backup)
{
    gboolean ret_val1 = hive_info_flush(in_hi, in_do_backup);
    gboolean ret_val2 = free_parts(in_hi);
    in_hi->type = HTYPE_UNLOADED;
    return ret_val1 && ret_val2;
}

gboolean
hive_info_is_valid(hive_info *in_hi)
{
    return in_hi->type > 0;
}

gboolean
hive_info_is_dirty(const hive_info *in_hi)
{
    return hive_is_dirty(hive_info_get_hive(in_hi));
}

hive_id
hive_info_get_id(const hive_info *in_hi)
{
    return in_hi->id;
}

void
hive_info_set_id(hive_info *in_hi, hive_id in_id)
{
    in_hi->id = in_id;
}

Hive*
hive_info_get_hive(const hive_info *in_hi)
{
    return in_hi->hive;
}

HiveType
hive_info_get_type(const hive_info *in_hi)
{
    return in_hi->type;
}

const char*
hive_info_get_filename(const hive_info *in_hi)
{
    return in_hi->filename;
}

HiveType
guess_hive_type (Hive *hdesc)
{
    HiveType result = HTYPE_UNKNOWN;
    ustring *hive_name = hive_get_name (hdesc);
    ustr_strdown(hive_name);
    const char *hname = ustr_as_utf8(hive_name);

    // So, let's guess what kind of hive this is, based on keys in its
    // root

    rra_debug(N_("Checking hive name %s of %s"),
              hname, hive_get_filename(hdesc));

    if (g_str_has_suffix(hname, "default"))
    {
        result = HTYPE_DEFAULT;
    }
    // @todo is the path part of the constant or does it change
    //       from user to user?
    else if (g_str_has_suffix(hname, "ntuser.dat"))
    {
        result = HTYPE_USER;
    }
    else if (g_str_has_suffix(hname, "sam"))
    {
        result = HTYPE_SAM;
    }
    else if (g_str_has_suffix(hname, "security"))
    {
        result = HTYPE_SECURITY;
    }
    else if (g_str_has_suffix(hname, "software"))
    {
        result = HTYPE_SOFTWARE;
    }
    else if (g_str_has_suffix(hname, "system"))
    {
        result = HTYPE_SYSTEM;
    }
    else if (g_str_has_suffix(hname, "userdiff"))
    {
        result = HTYPE_USERDIFF;
    }
    else if (g_str_has_suffix(hname, "usrclass.dat"))
    {
        result = HTYPE_USRCLASS;
    }

    if (result != HTYPE_UNKNOWN)
    {
        return result;
    }

    rra_debug(N_("Checking root key of file %s"),
              hive_get_filename(hdesc));

    /* Fallback look at the name of the root key */
    if (result == HTYPE_UNKNOWN)
    {
        if (key_cell_trav_path (hdesc, 0, "\\SAM"))
        {
            result = HTYPE_SAM;
        }
        else if (key_cell_trav_path (hdesc, 0, "\\ControlSet"))
        {
            result = HTYPE_SYSTEM;
        }
        else if (key_cell_trav_path (hdesc, 0, "\\Policy"))
        {
            result = HTYPE_SECURITY;
        }
        else if (key_cell_trav_path (hdesc, 0, "\\Microsoft"))
        {
            result = HTYPE_SOFTWARE;
        }
    }

    if (result != HTYPE_UNKNOWN)
    {
        return result;
    }

    rra_debug(N_("Checking filename of hive %s"),
              hive_get_filename(hdesc));

    /* Final fallback.  Look at the file name. */
    if (result == HTYPE_UNKNOWN)
    {
        ustring *filename = ustr_create(hive_get_filename(hdesc));
        ustr_strdown(filename);
        const char *str_fn = ustr_as_utf8(filename);

        if (g_str_has_suffix(str_fn, "default"))
        {
            result = HTYPE_DEFAULT;
        }
        else if (g_str_has_suffix(str_fn, "ntuser.dat"))
        {
            result = HTYPE_USER;
        }
        else if (g_str_has_suffix(str_fn, "sam"))
        {
            result = HTYPE_SAM;
        }
        else if (g_str_has_suffix(str_fn, "security"))
        {
            result = HTYPE_SECURITY;
        }
        else if (g_str_has_suffix(str_fn, "software"))
        {
            result = HTYPE_SOFTWARE;
        }
        else if (g_str_has_suffix(str_fn, "system"))
        {
            result = HTYPE_SYSTEM;
        }
        else if (g_str_has_suffix(str_fn, "userdiff"))
        {
            result = HTYPE_USERDIFF;
        }
        else if (g_str_has_suffix(str_fn, "usrclass.dat"))
        {
            result = HTYPE_USRCLASS;
        }

        ustr_free(filename);
    }

    ustr_free(hive_name);

    return result;
}
