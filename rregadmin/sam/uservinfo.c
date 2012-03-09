/*
 * Authors:     Sean Loaring
 *              Petter Nordahl-Hagen
 *              James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2007 Racemi Inc
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

#include <stdio.h>

#include <rregadmin/sam/uservinfo.h>

#include <rregadmin/hive/value_cell.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/malloc.h>

typedef enum
{
    UV_SECDESC = 0,
    UV_USERNAME = 1,
    UV_FULLNAME = 2,
    UV_COMMENT = 3,
    UV_USER_COMMENT = 4,
    UV_UNKNOWN2 = 5, /* mostly empty. ASPNET has it set */
    UV_HOMEDIR = 6,
    UV_HOMEDIRCONNECT = 7,
    UV_SCRIPTPATH = 8,
    UV_PROFILEPATH = 9,
    UV_WORKSTATIONS = 10,
    UV_HOURS_ALLOWED = 11,
    UV_UNKNOWN3 = 12, /* hex(01,02,00,00,07,00,00,00) in all test data */
    UV_LMPW = 13,
    UV_NTPW = 14,
    UV_EXTRAPW1 = 15, /* starts with 0x00010001 like LMPW nd NTPW */
    UV_EXTRAPW2 = 16, /* starts with 0x00010001 like LMPW nd NTPW */

    UV_FIELD_COUNT = 17,
} uv_field;

const char *uv_field_names[UV_FIELD_COUNT] = {
    "SECDESC",

    "USERNAME",
    "FULLNAME",
    "COMMENT",
    "USER_COMMENT",

    "UNKNOWN2",

    "HOMEDIR",
    "HOMEDIRCONNECT",
    "SCRIPTPATH",
    "PROFILEPATH",

    "WORKSTATIONS",
    "HOURS_ALLOWED",
    "UNKNOWN3",

    "LMPW",
    "NTPW",

    "EXTRAPW1",
    "EXTRAPW2"
};

struct fentry
{
    guint32 off;
    guint32 len;
    guint32 unknown;
} RRA_VERBATIM_STRUCT;

struct finfo_
{
    struct fentry e[UV_FIELD_COUNT];
    guint8 data[];
} RRA_VERBATIM_STRUCT;
typedef struct finfo_ finfo;

struct UserVInfo_
{
    ValueCell *vc;
    ustring *vals[UV_FIELD_COUNT]; /* mutable */
    SecurityDescriptor *secdesc; /* mutable */
};

static void
mark_dirty(UserVInfo *in_info)
{
    int i;
    for (i = 0; i < UV_FIELD_COUNT; i++)
    {
        if (in_info->vals[i] != NULL)
        {
            ustr_free(in_info->vals[i]);
            in_info->vals[i] = NULL;
        }
    }
    if (in_info->secdesc != NULL)
    {
        secdesc_free(in_info->secdesc);
        in_info->secdesc = NULL;
    }
}

static finfo*
get_finfo(const UserVInfo *in_info)
{
    return (finfo*)value_cell_get_data(in_info->vc);
}

UserVInfo*
uservinfo_new(ValueCell *in_cell)
{
    if (in_cell == NULL)
    {
        return NULL;
    }

    UserVInfo *ret_val = rra_new_type(UserVInfo);
    ret_val->vc = in_cell;
    return ret_val;
}

gboolean
uservinfo_free(UserVInfo *in_info)
{
    if (in_info == NULL)
    {
        return FALSE;
    }

    mark_dirty(in_info);

    rra_free_type(UserVInfo, in_info);

    return TRUE;
}

gboolean
uservinfo_is_valid(const UserVInfo *in_info)
{
    if (in_info == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

const SecurityDescriptor*
uservinfo_get_secdesc(const UserVInfo *in_info)
{
    if (in_info == NULL)
    {
        return NULL;
    }

    if (in_info->secdesc == NULL)
    {
        finfo *fi = get_finfo(in_info);
        gsize len = fi->e[UV_SECDESC].len;
        ((UserVInfo*)in_info)->secdesc =
            secdesc_new_parse_binary(fi->data + fi->e[UV_SECDESC].off, &len);
    }

    return in_info->secdesc;
}

static const ustring*
get_ustring_val(const UserVInfo *in_info, uv_field in_field)
{
    if (in_info == NULL)
    {
        return NULL;
    }

    if (in_info->vals[in_field] == NULL)
    {
        finfo *fi = get_finfo(in_info);
        ustring *tmp_str = ustr_new();
        ustr_strnset_type(tmp_str, USTR_TYPE_UTF16LE,
                          (char*)(fi->data + fi->e[in_field].off),
                          fi->e[in_field].len);
        ((UserVInfo*)in_info)->vals[in_field] = tmp_str;
    }

    return in_info->vals[in_field];
}

const ustring*
uservinfo_get_username(const UserVInfo *in_info)
{
    return get_ustring_val(in_info, UV_USERNAME);
}

const ustring*
uservinfo_get_fullname(const UserVInfo *in_info)
{
    return get_ustring_val(in_info, UV_FULLNAME);
}

const ustring*
uservinfo_get_comment(const UserVInfo *in_info)
{
    return get_ustring_val(in_info, UV_COMMENT);
}

const ustring*
uservinfo_get_user_comment(const UserVInfo *in_info)
{
    return get_ustring_val(in_info, UV_USER_COMMENT);
}

const ustring*
uservinfo_get_homedir(const UserVInfo *in_info)
{
    return get_ustring_val(in_info, UV_HOMEDIR);
}

const ustring*
uservinfo_get_scriptpath(const UserVInfo *in_info)
{
    return get_ustring_val(in_info, UV_SCRIPTPATH);
}

const ustring*
uservinfo_get_profilepath(const UserVInfo *in_info)
{
    return get_ustring_val(in_info, UV_PROFILEPATH);
}

gboolean
uservinfo_get_debug_output(const UserVInfo *in_info, ustring *ustr)
{
    if (in_info == NULL
        || ustr == NULL)
    {
        return FALSE;
    }

    int i;
    const finfo *fi = get_finfo(in_info);

    ustr_printfa(ustr, "Hexdump:\n");
    ustr_hexdumpa(ustr, value_cell_get_data(in_info->vc),
                  0, value_cell_get_data_length(in_info->vc), TRUE, TRUE);

    for (i = 0; i < UV_FIELD_COUNT; i++)
    {
        ustr_printfa(ustr,
                     "Field %s(%d): offset(%d), len(%d), unknown(%08x)\n",
                     uv_field_names[i], i,
                     fi->e[i].off, fi->e[i].len, fi->e[i].unknown);
        ustr_hexdumpa(ustr, fi->data, fi->e[i].off,
                      fi->e[i].off + fi->e[i].len, TRUE, TRUE);
    }

    ustr_printfa(ustr, "\n");

    return TRUE;
}

void
uservinfo_debug_print(const UserVInfo *in_info)
{
    ustring *ustr = ustr_new();
    if (uservinfo_get_debug_output(in_info, ustr))
    {
        fprintf (stderr, "%s", ustr_as_utf8(ustr));
    }
    ustr_free(ustr);
}
