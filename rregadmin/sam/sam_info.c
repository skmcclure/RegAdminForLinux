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
#include <stdio.h>

#include <glib.h>

#include <rregadmin/registry/registry.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/value_cell.h>
#include <rregadmin/util/value.h>
#include <rregadmin/util/timestamp.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/malloc.h>
#include <rregadmin/sam/sam_info.h>
#include <rregadmin/sam/user.h>
#include <rregadmin/sam/log.h>

/** The wrapper object for sam info.
 *
 * @ingroup sam_info_sam_group
 */
struct SamInfo_
{
    Registry *reg;
    ValueCell *vcell;
    ustring_list *names; /* mutable */
    GArray *ids; /* mutable */
};

/** The hive structure with SAM info.
 *
 * This is \\HKEY_LOCAL_MACHINE\\SAM\\SAM\\Domains\\Account\\F
 *
 * @ingroup sam_info_sam_group
 */
struct SamFKey_
{
    /** Id.
     *
     * It seems to be 0x02000100
     */
    guint32 id;
    /** Buffer
     *
     * Buffer area.
     * It seems to be 0.
     */
    guint32 u4;
    /** Unknown. May be machine SID
     */
    guint8 u8[8];
    /** auto increment (??)
     */
    guint32 autoinc;
    /** Unknown
     */
    guint32 u20;
    /** max password age
     */
    NTTIME max_password_age;
    /** min password age
     */
    NTTIME min_password_age;
    /** Unknown.
     */
    guint8 u40[8];
    /** account lockout duration
     */
    NTTIME lockout_duration;
    /** account lockout reset
     */
    NTTIME lockout_reset;
    /** Unknown
     */
    guint8 u64[8];
    /** the next RID that will be assigned
     */
    gint32 next_rid;
    guint32 flags;
    guint16 min_pass;
    guint16 enforce_password_history;
    guint16 lockout_threshold;
    /** Unknown
     */
    guint16 u78;
    /** Unknown
     */
    guint32 u80;
    /** Unknown
     */
    guint32 u84;
    /** Unknown
     */
    guint32 u88;
    /** Unknown
     */
    guint16 u92;
    /** Unknown
     */
    guint16 u94;
    /** Unknown
     */
    guint32 u96;
    /** Unknown
     */
    guint32 u100;
    /** This may very well be the obfuscated syskey
     */
    guint8 obf[48];

    /** Trailing data
     *
     * Always zeroes so far.
     */
    guint8 trailer[];
} RRA_VERBATIM_STRUCT;
typedef struct SamFKey_ SamFKey;

#define SAM_PATH_PREFIX "\\HKEY_LOCAL_MACHINE\\SAM\\SAM"
const char *SAMFKEY_PATH = SAM_PATH_PREFIX "\\Domains\\Account";
const char *SAMFKEY_VALUE = "F";
const char *USER_NAME_PATH =
    SAM_PATH_PREFIX "\\Domains\\Account\\Users\\Names\\";

static SamFKey*
get_fkey(const SamInfo *in_info)
{
    return (SamFKey*)value_cell_get_data(in_info->vcell);
}

static void
mark_dirty(SamInfo *in_info)
{
    if (in_info->names != NULL)
    {
        ustrlist_free(in_info->names);
        in_info->names = NULL;
    }

    if (in_info->ids != NULL)
    {
        g_array_free(in_info->ids, TRUE);
        in_info->ids = NULL;
    }
}

SamInfo*
sam_info_new(Registry *in_reg)
{
    if (in_reg == NULL)
    {
        return NULL;
    }

    ValueKeyCell *vc =
        registry_path_get_value(in_reg, SAMFKEY_PATH, SAMFKEY_VALUE);

    if (vc == NULL)
    {
        return NULL;
    }

    ValueCell *vcell = value_key_cell_get_value_cell(vc);

    if (vcell == NULL)
    {
        return NULL;
    }

    if (value_cell_get_data_length(vcell) < sizeof(SamFKey))
    {
        rra_warning("SamFKey data not appropriate size.  "
                    "Should be greater than or equal to %d, is %d.",
                    sizeof(SamFKey),
                    value_cell_get_data_length(vcell));
        return NULL;
    }

    SamInfo *ret_val = rra_new_type(SamInfo);

    ret_val->reg = in_reg;
    ret_val->vcell = vcell;

    return ret_val;
}

gboolean
sam_info_free(SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return FALSE;
    }

    mark_dirty(in_info);

    rra_free_type(SamInfo, in_info);

    return TRUE;
}

gboolean
sam_info_is_valid(const SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return FALSE;
    }

    SamFKey *fkey = get_fkey(in_info);

    if (fkey->id != 0x00010002)
    {
        return FALSE;
    }

    return TRUE;
}

gint32
sam_info_get_next_rid(const SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return -1;
    }

    return get_fkey(in_info)->next_rid;
}

guint32
sam_info_get_auto_increment(const SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return 0;
    }

    return get_fkey(in_info)->autoinc;
}

NTTIME
sam_info_get_max_password_age(const SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return nttime_get_error();
    }

    return get_fkey(in_info)->max_password_age;
}

NTTIME
sam_info_get_min_password_age(const SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return nttime_get_error();
    }

    return get_fkey(in_info)->min_password_age;
}

NTTIME
sam_info_get_lockout_duration(const SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return nttime_get_error();
    }

    return get_fkey(in_info)->lockout_duration;
}

NTTIME
sam_info_get_lockout_reset(const SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return nttime_get_error();
    }

    return get_fkey(in_info)->lockout_reset;
}

guint32
sam_info_get_flags(const SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return 0;
    }

    return get_fkey(in_info)->flags;
}

guint16
sam_info_get_min_pass(const SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return 0;
    }

    return get_fkey(in_info)->min_pass;
}

gboolean
sam_info_should_enforce_password_history(const SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return FALSE;
    }

    return get_fkey(in_info)->enforce_password_history > 0;
}

guint16
sam_info_get_lockout_threshold(const SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return 0;
    }

    return get_fkey(in_info)->lockout_threshold;
}

guint8*
sam_info_get_obfs_syskey(const SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return NULL;
    }

    return get_fkey(in_info)->obf;
}

int
sam_info_get_number_of_users(const SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return -1;
    }

    KeyCell *names_kc = registry_path_get_key(in_info->reg, USER_NAME_PATH);

    if (names_kc == NULL)
    {
        rra_warning("%s doesn't exist", USER_NAME_PATH);
        return -1;
    }

    return key_cell_get_number_of_subkeys(names_kc);
}

static void
read_names_and_ids(SamInfo *in_info)
{
    int i;
    KeyCell *names_kc = registry_path_get_key(in_info->reg, USER_NAME_PATH);
    in_info->names = ustrlist_new();
    in_info->ids = g_array_new(TRUE, TRUE, sizeof(gint32));

    for (i = 0; i < key_cell_get_number_of_subkeys(names_kc); i++)
    {
        KeyCell *subk = key_cell_get_subkey(names_kc, i);
        ustrlist_append(in_info->names, key_cell_get_name(subk));
        ValueKeyCell *vkey = key_cell_get_value_str(subk, "");
        Value *val = value_key_cell_get_val(vkey);
        dword_type id = value_get_as_dword(val);
        g_array_append_val(in_info->ids, id);
        value_free(val);
    }
}

const ustring_list*
sam_info_get_user_name_list (const SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return NULL;
    }

    if (in_info->names == NULL)
    {
        read_names_and_ids((SamInfo*)in_info);
    }

    return in_info->names;
}

const GArray*
sam_info_get_user_rid_list (const SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return NULL;
    }

    if (in_info->names == NULL)
    {
        read_names_and_ids((SamInfo*)in_info);
    }

    return in_info->ids;
}

SamUser*
sam_info_get_user_by_id(SamInfo *in_info, rid_type in_id)
{
    if (in_info == NULL)
    {
        return NULL;
    }

    return samuser_new_by_id(in_info, in_id);
}

SamUser*
sam_info_get_user_by_name(SamInfo *in_info, const ustring *in_name)
{
    if (in_info == NULL
        || in_name == NULL)
    {
        return NULL;
    }

    return samuser_new_by_name(in_info, in_name);
}

Registry*
sam_info_get_registry(SamInfo *in_info)
{
    if (in_info == NULL)
    {
        return NULL;
    }

    return in_info->reg;
}

void
sam_info_debug_print(const SamInfo *in_info)
{
    const SamFKey *key = get_fkey(in_info);
    ustring *ustr = ustr_new();

#define OUTPUT_PART(name)                                     \
    do {                                                      \
        ustr_printfa(ustr, "%s: ", #name);                    \
        ustr_hexstreama(ustr, (guint8*)&key->name, 0,         \
                        sizeof(key->name), ',');              \
        ustr_printfa(ustr, "\n");                             \
    } while(0)

    ustr_printfa(ustr, "Hexdump:\n");
    ustr_hexdumpa(ustr, value_cell_get_data(in_info->vcell),
                  0, value_cell_get_data_length(in_info->vcell), TRUE, TRUE);

    OUTPUT_PART(id);
    OUTPUT_PART(u4);
    OUTPUT_PART(u8);
    OUTPUT_PART(autoinc);
    OUTPUT_PART(u20);
    OUTPUT_PART(max_password_age);
    OUTPUT_PART(min_password_age);
    OUTPUT_PART(u40);
    OUTPUT_PART(lockout_duration);
    OUTPUT_PART(lockout_reset);
    OUTPUT_PART(u64);
    OUTPUT_PART(next_rid);
    OUTPUT_PART(flags);
    OUTPUT_PART(min_pass);
    OUTPUT_PART(enforce_password_history);
    OUTPUT_PART(lockout_threshold);
    OUTPUT_PART(u78);
    OUTPUT_PART(u80);
    OUTPUT_PART(u84);
    OUTPUT_PART(u88);
    OUTPUT_PART(u92);
    OUTPUT_PART(u94);
    OUTPUT_PART(u96);
    OUTPUT_PART(u100);
    OUTPUT_PART(obf);

    ustr_printfa(ustr, "\n");

    fprintf (stderr, "%s", ustr_as_utf8(ustr));
    ustr_free(ustr);
}
