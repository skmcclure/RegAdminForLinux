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

#include <rregadmin/sam/user.h>
#include <rregadmin/sam/log.h>
#include <rregadmin/sam/sam_info.h>
#include <rregadmin/sam/uservinfo.h>
#include <rregadmin/sam/userfinfo.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/malloc.h>
#include <rregadmin/registry/registry.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/value_key_cell.h>

struct SamUser_
{
    SamInfo *si;
    rid_type id;
    UserFInfo *ufi;
    UserVInfo *uvi;
};

static const char* user_path_prefix =
    "\\HKEY_LOCAL_MACHINE\\SAM\\SAM\\Domains\\Account\\Users";

SamUser*
samuser_new_by_id(SamInfo *in_si, rid_type in_id)
{
    if (in_si == NULL)
    {
        return NULL;
    }

    ustring *path = ustr_new();
    ustr_printf(path, "%s\\%08x", user_path_prefix, in_id);
    KeyCell *kc = registry_path_get_key(sam_info_get_registry(in_si),
                                        ustr_as_utf8(path));
    ustr_free(path);
    if (kc == NULL)
    {
        rra_warning("Couldn't get path %s\\%08x",
                    user_path_prefix, in_id);
        return NULL;
    }
    ValueKeyCell *v_vkc = key_cell_get_value_str(kc, "V");
    if (v_vkc == NULL)
    {
        rra_warning("Couldn't find 'V' value under %s\\%08x",
                    user_path_prefix, in_id);
        return NULL;
    }
    UserVInfo *uvi = uservinfo_new(value_key_cell_get_value_cell(v_vkc));

    ValueKeyCell *f_vkc = key_cell_get_value_str(kc, "F");
    if (f_vkc == NULL)
    {
        rra_warning("Couldn't find 'F' value under %s\\%08x",
                    user_path_prefix, in_id);
        return NULL;
    }
    UserFInfo *ufi = userfinfo_new(value_key_cell_get_value_cell(f_vkc));


    SamUser *ret_val = rra_new_type(SamUser);
    ret_val->si = in_si;
    ret_val->id = in_id;
    ret_val->ufi = ufi;
    ret_val->uvi = uvi;
    return ret_val;
}

static rid_type
find_id_by_name(SamInfo *in_si, const ustring *in_name)
{
    rid_type ret_val;
    ustring *path = ustr_new();
    ustr_printf(path, "%s\\Names\\%s", user_path_prefix,
                ustr_as_utf8(in_name));
    ValueKeyCell *vkc = registry_path_get_value(sam_info_get_registry(in_si),
                                                ustr_as_utf8(path), "");
    ustr_free(path);

    if (vkc == NULL)
    {
        return RID_INVALID;
    }

    Value *val = value_key_cell_get_val(vkc);
    ret_val = value_get_as_dword(val);
    value_free(val);

    return ret_val;
}

SamUser*
samuser_new_by_name(SamInfo *in_si, const ustring *in_name)
{
    if (in_si == NULL
        || in_name == NULL)
    {
        return NULL;
    }

    return samuser_new_by_id(in_si, find_id_by_name(in_si, in_name));
}

gboolean
samuser_free(SamUser *in_user)
{
    if (in_user == NULL)
    {
        return FALSE;
    }

    in_user->si = NULL;
    in_user->id = RID_INVALID;
    userfinfo_free(in_user->ufi);
    in_user->ufi = NULL;
    uservinfo_free(in_user->uvi);
    in_user->uvi = NULL;

    rra_free_type(SamUser, in_user);

    return TRUE;
}

gboolean
samuser_is_valid(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

rid_type
samuser_get_rid(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return RID_INVALID;
    }

    return userfinfo_get_rid(in_user->ufi);
}

NTTIME
samuser_get_last_login(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return nttime_get_error();
    }

    return userfinfo_get_last_login(in_user->ufi);
}

NTTIME
samuser_get_last_password_change(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return nttime_get_error();
    }

    return userfinfo_get_last_password_change(in_user->ufi);
}

NTTIME
samuser_get_account_expires(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return nttime_get_error();
    }

    return userfinfo_get_account_expires(in_user->ufi);
}

NTTIME
samuser_get_last_invalid_password(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return nttime_get_error();
    }

    return userfinfo_get_last_invalid_password(in_user->ufi);
}

acb_type
samuser_get_acb(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return nttime_get_error();
    }

    return userfinfo_get_acb(in_user->ufi);
}

const ustring*
samuser_get_acb_string(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return NULL;
    }

    return userfinfo_get_acb_string(in_user->ufi);
}

int
samuser_get_bad_login_count(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return -1;
    }

    return userfinfo_get_bad_login_count(in_user->ufi);
}

int
samuser_get_number_of_logins(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return -1;
    }

    return userfinfo_get_number_of_logins(in_user->ufi);
}

country_code_type
samuser_get_country_code(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return COUNTRY_CODE_ERROR_VALUE;
    }

    return userfinfo_get_country_code(in_user->ufi);
}

const ustring*
samuser_get_username(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return NULL;
    }

    return uservinfo_get_username(in_user->uvi);
}

const ustring*
samuser_get_fullname(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return NULL;
    }

    return uservinfo_get_fullname(in_user->uvi);
}

const ustring*
samuser_get_comment(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return NULL;
    }

    return uservinfo_get_comment(in_user->uvi);
}

const ustring*
samuser_get_user_comment(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return NULL;
    }

    return uservinfo_get_user_comment(in_user->uvi);
}

const ustring*
samuser_get_homedir(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return NULL;
    }

    return uservinfo_get_homedir(in_user->uvi);
}

const ustring*
samuser_get_scriptpath(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return NULL;
    }

    return uservinfo_get_scriptpath(in_user->uvi);
}

const ustring*
samuser_get_profilepath(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return NULL;
    }

    return uservinfo_get_profilepath(in_user->uvi);
}

const SecurityDescriptor*
samuser_get_secdesc(const SamUser *in_user)
{
    if (in_user == NULL)
    {
        return NULL;
    }

    return uservinfo_get_secdesc(in_user->uvi);
}

gboolean
samuser_get_debug_output(const SamUser *in_user, ustring *in_ustr)
{
    if (in_user == NULL
        || in_ustr == NULL)
    {
        return FALSE;
    }

    ustr_printfa (in_ustr, "%s (%d):\n",
                  ustr_as_utf8(samuser_get_username(in_user)),
                  samuser_get_rid(in_user));
    ustr_printfa (in_ustr, "  Fullname: %s\n",
                  ustr_as_utf8(samuser_get_fullname(in_user)));
    ustr_printfa (in_ustr, "  Comment: %s\n",
                  ustr_as_utf8(samuser_get_comment(in_user)));
    ustr_printfa (in_ustr, "  User_Comment: %s\n",
                  ustr_as_utf8(samuser_get_user_comment(in_user)));
    ustr_printfa (in_ustr, "  Homedir: %s\n",
                  ustr_as_utf8(samuser_get_homedir(in_user)));
    ustr_printfa (in_ustr, "  Scriptpath: %s\n",
                  ustr_as_utf8(samuser_get_scriptpath(in_user)));
    ustr_printfa (in_ustr, "  Profilepath: %s\n",
                  ustr_as_utf8(samuser_get_profilepath(in_user)));
    ustr_printfa (in_ustr, "  SecDesc: %s\n",
                  secdesc_to_sddl(samuser_get_secdesc(in_user)));

    ustring *ts;

    ts = nttime_to_str(samuser_get_last_login(in_user));
    ustr_printfa (in_ustr, "  LastLogin: %s\n", ustr_as_utf8(ts));
    ustr_free(ts);
    ts = nttime_to_str(samuser_get_last_password_change(in_user));
    ustr_printfa (in_ustr, "  LastPasswordChange: %s\n", ustr_as_utf8(ts));
    ustr_free(ts);
    ts = nttime_to_str(samuser_get_account_expires(in_user));
    ustr_printfa (in_ustr, "  AccountExpires: %s\n", ustr_as_utf8(ts));
    ustr_free(ts);
    ts = nttime_to_str(samuser_get_last_invalid_password(in_user));
    ustr_printfa (in_ustr, "  LastInvalidPassword: %s\n", ustr_as_utf8(ts));
    ustr_free(ts);

    ustr_printfa (in_ustr, "  Acb: %s\n",
                  ustr_as_utf8(samuser_get_acb_string(in_user)));
    ustr_printfa (in_ustr, "  BadLoginCount: %d\n",
                  samuser_get_bad_login_count(in_user));
    ustr_printfa (in_ustr, "  NumberOfLogins: %d\n",
                  samuser_get_number_of_logins(in_user));
    ustr_printfa (in_ustr, "  CountryCode: %d\n",
                  samuser_get_country_code(in_user));

    return TRUE;
}

void
samuser_debug_print(const SamUser *in_user)
{
    ustring *ustr = ustr_new();
    if (samuser_get_debug_output(in_user, ustr))
    {
        fprintf (stderr, "%s", ustr_as_utf8(ustr));
    }
    ustr_free(ustr);
}
