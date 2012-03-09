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

#include <rregadmin/sam/userfinfo.h>

#include <rregadmin/hive/value_cell.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/malloc.h>

struct UserFInfo_
{
    ValueCell *vc;
    ustring *acb; /* mutable */
};

struct finfo_
{
    guint32 id;
    /** Unknown
     *
     * @note is 0 in all seen cases
     */
    guint32 u4;
    NTTIME last_login;
    NTTIME unknown_time;
    NTTIME last_pw_change;
    NTTIME account_expires;
    NTTIME last_invalid_pass;
    rid_type rid;
    /** Unknown
     *
     * @note is 0x00000201 in all seen cases
     */
    guint32 u52;
    acb_type acb;
    /** Unknown
     *
     * @note is 0 in all seen cases
     */
    guint16 u58;
    country_code_type country_code;
    /** Unknown
     *
     * @note is 0 in all seen cases
     */
    guint16 u62;
    guint16 bad_login_count;
    guint16 no_login;
    /** Unknown
     *
     * @note is 0 or 1 in all seen cases
     */
    guint16 u68;
    /** Unknown
     *
     * @note is 0 in all seen cases
     */
    guint16 u70;
    /** Unknown
     *
     * @note is 0 in all seen cases
     */
    guint16 u72;
    /** Unknown
     *
     * @note seen values include 0x0000, 0x0008, 0x7c82, 0x000d, 0x0007,
     *       0xffff
     */
    guint16 u74;
} RRA_VERBATIM_STRUCT;
typedef struct finfo_ finfo;

static const char *acb_field_names[16] = {
    "disabled",
    "homedir-required",
    "passwd-not-required",
    "temporary-duplicate-account",
    "normal-account",
    "nms-account",
    "domain-trust-account",
    "wks-trust-account",
    "srv-trust-account",
    "password-no-expire",
    "auto-lockout",
    "unknown-0x0800",
    "unknown-0x1000",
    "unknown-0x2000",
    "unknown-0x4000",
    "unknown-0x8000",
};

static finfo*
get_finfo(const UserFInfo *in_info)
{
    return (finfo*)value_cell_get_data(in_info->vc);
}

UserFInfo*
userfinfo_new(ValueCell *in_cell)
{
    if (in_cell == NULL)
    {
        return NULL;
    }

    UserFInfo *ret_val = rra_new_type(UserFInfo);
    ret_val->vc = in_cell;
    return ret_val;
}

gboolean
userfinfo_free(UserFInfo *in_info)
{
    if (in_info == NULL)
    {
        return FALSE;
    }

    rra_free_type(UserFInfo, in_info);

    return TRUE;
}

gboolean
userfinfo_is_valid(const UserFInfo *in_info)
{
    if (in_info == NULL)
    {
        return FALSE;
    }

    if (get_finfo(in_info)->id != 0x00010002)
    {
        return FALSE;
    }

    return TRUE;
}

NTTIME
userfinfo_get_last_login(const UserFInfo *in_info)
{
    if (in_info == NULL)
    {
        return nttime_get_error();
    }

    return get_finfo(in_info)->last_login;
}

NTTIME
userfinfo_get_last_password_change(const UserFInfo *in_info)
{
    if (in_info == NULL)
    {
        return nttime_get_error();
    }

    return get_finfo(in_info)->last_pw_change;
}

NTTIME
userfinfo_get_account_expires(const UserFInfo *in_info)
{
    if (in_info == NULL)
    {
        return nttime_get_error();
    }

    return get_finfo(in_info)->account_expires;
}

NTTIME
userfinfo_get_last_invalid_password(const UserFInfo *in_info)
{
    if (in_info == NULL)
    {
        return nttime_get_error();
    }

    return get_finfo(in_info)->last_invalid_pass;
}

rid_type
userfinfo_get_rid(const UserFInfo *in_info)
{
    if (in_info == NULL)
    {
        return (rid_type)-1;
    }

    return get_finfo(in_info)->rid;
}

acb_type
userfinfo_get_acb(const UserFInfo *in_info)
{
    if (in_info == NULL)
    {
        return ACB_ERROR_VALUE;
    }

    return get_finfo(in_info)->acb;
}

const ustring*
userfinfo_get_acb_string(const UserFInfo *in_info)
{
    if (in_info == NULL)
    {
        return NULL;
    }

    if (in_info->acb == NULL)
    {
        int i;
        finfo *fi = get_finfo(in_info);
        ((UserFInfo*)in_info)->acb = ustr_new();
        for (i = 15; i >= 0; i--)
        {
            if ((fi->acb & (1 << i)) != 0)
            {
                ustr_printfa(in_info->acb, "%s,", acb_field_names[i]);
            }
        }

        if (ustr_strlen(in_info->acb) > 0)
        {
            ustr_trim_back(in_info->acb, 1);
        }
    }

    return in_info->acb;
}

int
userfinfo_get_bad_login_count(const UserFInfo *in_info)
{
    if (in_info == NULL)
    {
        return -1;
    }

    return get_finfo(in_info)->bad_login_count;
}

int
userfinfo_get_number_of_logins(const UserFInfo *in_info)
{
    if (in_info == NULL)
    {
        return -1;
    }

    return get_finfo(in_info)->no_login;
}

country_code_type
userfinfo_get_country_code(const UserFInfo *in_info)
{
    if (in_info == NULL)
    {
        return COUNTRY_CODE_ERROR_VALUE;
    }

    return get_finfo(in_info)->country_code;
}

gboolean
userfinfo_get_debug_output(const UserFInfo *in_info, ustring *ustr)
{
    if (in_info == NULL
        || ustr == NULL)
    {
        return FALSE;
    }

    const finfo *fi = get_finfo(in_info);

#define OUTPUT_PART(name)                                     \
    do {                                                      \
        ustr_printfa(ustr, "%s: ", #name);                    \
        ustr_hexstreama(ustr, (guint8*)&fi->name, 0,         \
                        sizeof(fi->name), ',');              \
        ustr_printfa(ustr, "\n");                             \
    } while(0)

    ustr_printfa(ustr, "Hexdump:\n");
    ustr_hexdumpa(ustr, value_cell_get_data(in_info->vc),
                  0, value_cell_get_data_length(in_info->vc), TRUE, TRUE);

    OUTPUT_PART(id);
    OUTPUT_PART(u4);
    OUTPUT_PART(last_login);
    OUTPUT_PART(unknown_time);
    OUTPUT_PART(last_pw_change);
    OUTPUT_PART(account_expires);
    OUTPUT_PART(last_invalid_pass);
    OUTPUT_PART(rid);
    OUTPUT_PART(u52);
    OUTPUT_PART(acb);
    OUTPUT_PART(u58);
    OUTPUT_PART(country_code);
    OUTPUT_PART(u62);
    OUTPUT_PART(bad_login_count);
    OUTPUT_PART(no_login);
    OUTPUT_PART(u68);
    OUTPUT_PART(u70);
    OUTPUT_PART(u72);
    OUTPUT_PART(u74);

    ustr_printfa(ustr, "\n");
    return TRUE;
}

void
userfinfo_debug_print(const UserFInfo *in_info)
{
    ustring *ustr = ustr_new();
    if (userfinfo_get_debug_output(in_info, ustr))
    {
        fprintf (stderr, "%s", ustr_as_utf8(ustr));
    }
    ustr_free(ustr);
}
