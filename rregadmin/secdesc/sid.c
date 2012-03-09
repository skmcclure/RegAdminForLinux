/*
  Copyright 2007 Racemi Inc
  Copyright (C) Stefan (metze) Metzmacher 	2002-2004
  Copyright (C) Andrew Tridgell 		1992-2004
  Copyright (C) Jeremy Allison  		1999

  This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>

#include <rregadmin/secdesc/sid.h>

#include <rregadmin/util/ustring.h>
#include <rregadmin/util/malloc.h>

#define SUBAUTH_MAX_COUNT 15

struct sid_header
{
    guint8 rev;
    gint8 num_auths;/* [range(0 15)] */
    guint16 buf1;
    guint32 id;
} RRA_VERBATIM_STRUCT;

struct sid_binary
{
    struct sid_header head;
    guint32 sub_auths[];
} RRA_VERBATIM_STRUCT;

struct Sid_
{
    guint8 rev;
    guint32 id;
    ustring *sddl; /* mutable */
    GArray *sub_auths;
};

struct sid_alias_mapping
{
    const char *code;
    const char *sid;
    const char *descrip;
};

#define UNKNOWN_SID "S-1-1-1-1"

#define SDDL_CERT_SERV_ADMINS UNKNOWN_SID "-1"
#define SDDL_DOMAIN_ADMINS UNKNOWN_SID "-2"
#define SDDL_DOMAIN_COMPUTERS UNKNOWN_SID "-3"
#define SDDL_DOMAIN_DOMAIN_CONTROLLERS UNKNOWN_SID "-4"
#define SDDL_DOMAIN_GUESTS UNKNOWN_SID "-5"
#define SDDL_DOMAIN_USERS UNKNOWN_SID "-6"
#define SDDL_ENTERPRISE_ADMINS UNKNOWN_SID "-7"
#define SDDL_ML_HIGH UNKNOWN_SID "-8"
#define SDDL_LOCAL_ADMINS UNKNOWN_SID "-9"
#define SDDL_LOCAL_GUEST UNKNOWN_SID "-10"
#define SSDL_ML_LOW UNKNOWN_SID "-11"
#define SDDL_ML_MEDIUM UNKNOWN_SID "-12"
#define SDDL_NETWORK_CONFIGURATION_OPS UNKNOWN_SID "-13"
#define SDDL_GROUP_POLICY_ADMINS UNKNOWN_SID "-14"
#define SDDL_SCHEMA_ADMINS UNKNOWN_SID "-15"
#define SDDL_ML_SYSTEM UNKNOWN_SID "-16"
#define SDDL_SERVICE UNKNOWN_SID "-17"

static const struct sid_alias_mapping sid_codes[] =
{
    { "AN", SID_NT_ANONYMOUS, "Anonymous logon"},
    { "AO", SID_BUILTIN_ACCOUNT_OPERATORS, "Account operators" },
    { "AU", SID_NT_AUTHENTICATED_USERS, "Authenticade users" },
    { "BA", SID_BUILTIN_ADMINISTRATORS, "Built-in admins" },
    { "BG", SID_BUILTIN_GUESTS, "Built-in guests" },
    { "BO", SID_BUILTIN_BACKUP_OPERATORS, "Backup operators" },
    { "BU", SID_BUILTIN_USERS, "Built-in users" },
    { "CA", SDDL_CERT_SERV_ADMINS, "Certificate server admins" },
    { "CG", SID_CREATOR_GROUP, "Creator group" },
    { "CO", SID_CREATOR_OWNER, "Creator owner" },
    { "DA", SDDL_DOMAIN_ADMINS, "Domain admins" },
    { "DC", SDDL_DOMAIN_COMPUTERS, "Domain computers" },
    { "DD", SDDL_DOMAIN_DOMAIN_CONTROLLERS, "Domain controllers" },
    { "DG", SDDL_DOMAIN_GUESTS, "Domain guests" },
    { "DU", SDDL_DOMAIN_USERS, "Domain users" },
    { "EA", SDDL_ENTERPRISE_ADMINS, "Enterprise admins" },
    { "ED", SID_NT_ENTERPRISE_DCS, "Enterprise domain controllers" },
    { "HI", SDDL_ML_HIGH, "High integrity level" },
    { "IU", SID_NT_INTERACTIVE, "Interactively logged-on user" },
    { "LA", SDDL_LOCAL_ADMINS, "Local administrator" },
    { "LG", SDDL_LOCAL_GUEST, "Local guest" },
    { "LS", SID_NT_LOCAL_SERVICE, "Local service account" },
    { "LW", SSDL_ML_LOW, "Low integrity level" },
    { "ME", SDDL_ML_MEDIUM, "Medium integrity" },
    { "NO", SDDL_NETWORK_CONFIGURATION_OPS, "Network config operators" },
    { "NS", SID_NT_NETWORK_SERVICE, "Network service account" },
    { "NU", SID_NT_NETWORK, "Network logon user" },
    { "PA", SDDL_GROUP_POLICY_ADMINS, "Group Policy admins" },
    { "PO", SID_BUILTIN_PRINT_OPERATORS, "Printer operators" },
    { "PS", SID_NT_SELF, "Personal self" },
    { "PU", SID_BUILTIN_POWER_USERS, "Powen users" },
    { "RC", SID_NT_RESTRICTED, "Restricted code" },
    { "RD", SID_NT_TERMINAL_SERVER_USERS, "Terminal server users" },
    { "RE", SID_BUILTIN_REPLICATOR, "Replicator" },
    { "RS", SID_BUILTIN_RAS_SERVERS, "RAS servers group"},
    { "RU", SID_BUILTIN_PREW2K, "Alias for previous Windows 2000" },
    { "SA", SDDL_SCHEMA_ADMINS, "Schema admins" },
    { "SI", SDDL_ML_SYSTEM, "System integrity level" },
    { "SO", SID_BUILTIN_SERVER_OPERATORS, "Server operators" },
    { "SU", SDDL_SERVICE, "Service logon user" },
    { "SY", SID_NT_SYSTEM, "Local system" },
    { "WD", SID_WORLD, "Everyone" },
};
static const int sid_codes_count = G_N_ELEMENTS(sid_codes);

static void
mark_dirty(Sid *in_sid)
{
    if (in_sid->sddl != NULL)
    {
        ustr_free(in_sid->sddl);
        in_sid->sddl = NULL;
    }
}

static guint32 allocation_count = 0;

guint32
sid_allocation_count(void)
{
    return allocation_count;
}

Sid*
sid_new(void)
{
    Sid *ret_val;

    ret_val = rra_new_type(Sid);

    ret_val->rev = SID_DEFAULT_REV;
    ret_val->id = SID_DEFAULT_ID;

    ret_val->sub_auths = NULL;
    ret_val->sddl = NULL;

    allocation_count++;

    return ret_val;
}

Sid*
sid_new_id(rid_type in_id)
{
    Sid *ret_val = sid_new();
    ret_val->id = in_id;
    return ret_val;
}

gboolean
sid_free(Sid *in_sid)
{
    if (in_sid == NULL)
    {
        return FALSE;
    }

    mark_dirty(in_sid);

    if (in_sid->sub_auths != NULL)
    {
        g_array_free(in_sid->sub_auths, TRUE);
    }

    rra_free_type(Sid, in_sid);
    allocation_count--;

    return TRUE;
}

Sid*
sid_new_parse_sddl(const char **in_str)
{
    Sid *ret_val;
    guint32 rev;
    guint32 ia;
    char *p;
    int i;

    if (in_str == NULL || *in_str == NULL)
    {
        return NULL;
    }

    /* Handle two letter codes */
    for (i = 0; i < sid_codes_count; i++)
    {
        int code_len = strlen(sid_codes[i].code);
        if (strncasecmp(*in_str, sid_codes[i].code, code_len) == 0)
        {
            *in_str += code_len;
            const char *code = sid_codes[i].sid;
            return sid_new_parse_sddl(&code);
        }
    }

    if (strncasecmp(*in_str, "S-", 2))
    {
        return NULL;
    }
    *in_str += 2;

    rev = strtol(*in_str, &p, 10);
    if (*p != '-')
    {
        return NULL;
    }
    *in_str = p+1;

    ia = strtol(*in_str, &p, 10);
    if (p == *in_str)
    {
        return NULL;
    }
    *in_str = p;

    ret_val = sid_new_id(ia);
    ret_val->rev = rev;

    while (*in_str[0] != '\0')
    {
        if (*in_str[0] != '-')
        {
            break;
        }
        (*in_str)++;

        sid_push_subauth(ret_val, strtoul(*in_str, &p, 10));
        if (p == *in_str)
        {
            sid_free(ret_val);
            return NULL;
        }
        *in_str = p;
    }

    return ret_val;
}

Sid*
sid_new_parse_sddl_quick(const char *in_str)
{
    if (in_str == NULL)
    {
        return NULL;
    }

    return sid_new_parse_sddl(&in_str);
}

Sid*
sid_new_parse_binary(const guint8 *in_data, gssize *in_size)
{
    if (in_data == NULL
        || in_size == NULL)
    {
        return NULL;
    }

    if (*in_size < sizeof(struct sid_binary))
    {
        return NULL;
    }

    struct sid_binary *sb = (struct sid_binary*)in_data;
    int expected_size = (sizeof(struct sid_binary)
                         + (sb->head.num_auths * sizeof(guint32)));

    if (*in_size < expected_size)
    {
        return NULL;
    }

    int i;
    Sid *ret_val = sid_new();
    ret_val->rev = sb->head.rev;
    ret_val->id = sb->head.id;

    for (i = 0; i < sb->head.num_auths; i++)
    {
        sid_push_subauth(ret_val, sb->sub_auths[i]);
    }

    *in_size -= expected_size;

    return ret_val;
}

Sid*
sid_copy(const Sid *in_sid)
{
    Sid *ret_val;
    int i;

    if (in_sid == NULL)
    {
        return NULL;
    }

    ret_val = sid_new();
    if (ret_val == NULL)
    {
        return NULL;
    }

    ret_val->rev = in_sid->rev;
    ret_val->id = in_sid->id;

    for (i = 0; i < sid_get_subauth_count(in_sid); i++)
    {
        sid_push_subauth(ret_val, sid_get_subauth(in_sid, i));
    }

    return ret_val;
}

sid_rev
sid_get_rev(const Sid *in_sid)
{
    if (in_sid == NULL)
    {
        return SID_ERROR_REV;
    }

    return in_sid->rev;
}

rid_type
sid_get_id(const Sid *in_sid)
{
    if (in_sid == NULL)
    {
        return SID_ERROR_ID;
    }

    return in_sid->id;
}

gboolean
sid_set_id(Sid *in_sid, rid_type in_id)
{
    if (in_sid == NULL)
    {
        return FALSE;
    }

    in_sid->id = in_id;

    mark_dirty(in_sid);

    return TRUE;
}

sid_auth_count
sid_get_subauth_count(const Sid *in_sid)
{
    if (in_sid == NULL)
    {
        return SID_ERROR_AUTH_COUNT;
    }

    if (in_sid->sub_auths == NULL)
    {
        return 0;
    }
    else
    {
        return in_sid->sub_auths->len;
    }
}

static void
assure_subauth_array(Sid *in_sid)
{
    if (in_sid->sub_auths == NULL)
    {
        in_sid->sub_auths = g_array_new(FALSE, TRUE, sizeof(rid_type));
    }
}

gboolean
sid_push_subauth(Sid *in_sid, rid_type in_auth)
{
    if (in_sid == NULL)
    {
        return FALSE;
    }
    assure_subauth_array(in_sid);

    if (sid_get_subauth_count(in_sid) >= SUBAUTH_MAX_COUNT)
    {
        return FALSE;
    }

    g_array_append_val(in_sid->sub_auths, in_auth);

    mark_dirty(in_sid);

    return TRUE;
}

gboolean
sid_pop_subauth(Sid *in_sid)
{
    if (in_sid == NULL)
    {
        return FALSE;
    }

    if (in_sid->sub_auths == NULL)
    {
        return FALSE;
    }

    if (sid_get_subauth_count(in_sid) == 0)
    {
        return FALSE;
    }

    g_array_remove_index(in_sid->sub_auths, sid_get_subauth_count(in_sid) - 1);

    mark_dirty(in_sid);

    return TRUE;
}

rid_type
sid_get_last_subauth(const Sid *in_sid)
{
    return sid_get_subauth(in_sid, sid_get_subauth_count(in_sid) - 1);
}

rid_type
sid_get_subauth(const Sid *in_sid, gint8 in_index)
{
    if (in_sid == NULL)
    {
        return SID_ERROR_ID;
    }

    if (in_index >= sid_get_subauth_count(in_sid)
        || in_index < 0)
    {
        return SID_ERROR_ID;
    }

    return g_array_index(in_sid->sub_auths, rid_type, in_index);
}

int
sid_compare(const Sid *sid1, const Sid *sid2)
{
    int i;

    if (sid1 == sid2)
        return 0;
    if (!sid1)
        return -1;
    if (!sid2)
        return 1;

    if (sid1->rev != sid2->rev)
    {
        return sid1->rev - sid2->rev;
    }

    if (sid1->id != sid2->id)
    {
        return sid1->id - sid2->id;
    }

    /* Compare most likely different rids, first: i.e start at end */
    if (sid_get_subauth_count(sid1) != sid_get_subauth_count(sid2))
    {
        return sid_get_subauth_count(sid1) - sid_get_subauth_count(sid2);
    }

    for (i = sid_get_subauth_count(sid1) - 1; i >= 0; --i)
    {
        if (sid_get_subauth(sid1, i) != sid_get_subauth(sid2, i))
        {
            return sid_get_subauth(sid1, i) - sid_get_subauth(sid2, i);
        }
    }

    return 0;
}

gboolean
sid_equal(const Sid *sid1, const Sid *sid2)
{
    return sid_compare(sid1, sid2) == 0;
}

gboolean
sid_write_to_binary(const Sid *in_sid, GByteArray *in_arr)
{
    int i;
    struct sid_binary s;
    s.head.rev = sid_get_rev(in_sid);
    s.head.buf1 = 0;
    s.head.id = sid_get_id(in_sid);
    s.head.num_auths = sid_get_subauth_count(in_sid);

    g_byte_array_append(in_arr, (const guint8*)&s, sizeof(struct sid_binary));

    for (i = 0; i < s.head.num_auths; i++)
    {
        rid_type id = sid_get_subauth(in_sid, i);
        g_byte_array_append(in_arr, (const guint8*)&id, sizeof(rid_type));
    }

    return TRUE;
}

GByteArray*
sid_to_binary(const Sid *in_sid)
{
    GByteArray *ret_val = g_byte_array_new();
    if (!sid_write_to_binary(in_sid, ret_val))
    {
        g_byte_array_free(ret_val, TRUE);
        return NULL;
    }
    return ret_val;
}

const char*
sid_to_sddl(const Sid *in_sid)
{
    int i;

    if (!in_sid)
    {
        return "(NULL SID)";
    }

    if (in_sid->sddl == NULL)
    {
        ((Sid*)in_sid)->sddl = ustr_new();

        ustr_printf(in_sid->sddl, "S-%u-%lu",
                    (unsigned int)in_sid->rev, (unsigned long)in_sid->id);

        for (i = 0; i < sid_get_subauth_count(in_sid); i++)
        {
            ustr_printfa(in_sid->sddl, "-%lu",
                         (unsigned long)sid_get_subauth(in_sid, i));
        }
    }

    /* Handle two letter codes */
    for (i = 0; i < sid_codes_count; i++)
    {
        if (strcasecmp(ustr_as_utf8(in_sid->sddl), sid_codes[i].sid) == 0)
        {
            ustr_strset(in_sid->sddl, sid_codes[i].code);
            break;
        }
    }

    return ustr_as_utf8(in_sid->sddl);
}
