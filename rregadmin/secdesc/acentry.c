/*
  Copyright 2007 Racemi Inc
  Copyright (C) Andrew Tridgell 		2004

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

#include <rregadmin/config.h>

#include <string.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/secdesc/acentry.h>
#include <rregadmin/secdesc/sid.h>
#include <rregadmin/secdesc/log.h>
#include <rregadmin/util/flag_map.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/malloc.h>

struct acentry_header
{
    guint8 type;
    guint8 flags;
    guint16 size;
    guint32 access_mask;
} RRA_VERBATIM_STRUCT;

struct acentry_binary
{
    struct acentry_header head;
    guint8 trustee[];
} RRA_VERBATIM_STRUCT;

struct ACEntry_
{
    struct acentry_header head;
    ustring *sddl; /* mutable */
    Sid *trustee;
};

static void
mark_dirty(ACEntry *in_ace)
{
    if (in_ace->sddl != NULL)
    {
        ustr_free(in_ace->sddl);
        in_ace->sddl = NULL;
    }
}

static guint32 allocation_count = 0;

guint32
acentry_allocation_count(void)
{
    return allocation_count;
}

/* Must keep shorter aliases after longer ones. */
static const FlagMap ace_types_map[] =
{
    { "AU", N_("SystemAudit"),
      N_("Please add description"),
      SEC_ACE_TYPE_SYSTEM_AUDIT },
    { "AL", N_("SystemAlarm"),
      N_("Please add description"),
      SEC_ACE_TYPE_SYSTEM_ALARM },
    { "OA", N_("AccessAllowedObject"),
      N_("Please add description"),
      SEC_ACE_TYPE_ACCESS_ALLOWED_OBJECT },
    { "OD", N_("AccessDeniedObject"),
      N_("Please add description"),
      SEC_ACE_TYPE_ACCESS_DENIED_OBJECT },
    { "OU", N_("SystemAuditObject"),
      N_("Please add description"),
      SEC_ACE_TYPE_SYSTEM_AUDIT_OBJECT },
    { "OL", N_("SystemAlarmObject"),
      N_("Please add description"),
      SEC_ACE_TYPE_SYSTEM_ALARM_OBJECT },

    { "A",  N_("AccessAllowed"),
      N_("Please add description"),
      SEC_ACE_TYPE_ACCESS_ALLOWED },
    { "D",  N_("AccessDenied"),
      N_("Please add description"),
      SEC_ACE_TYPE_ACCESS_DENIED },

    /* These appear but don't know what they are */
    { "UNKNOWN-0x20", N_("UNKNOWN-0x20"),
      N_("Please add description"), 0x20 },
    { "UNKNOWN-0x23", N_("UNKNOWN-0x23"),
      N_("Please add description"), 0x23 },
    { "UNKNOWN-0x53", N_("UNKNOWN-0x53"),
      N_("Please add description"), 0x53 },

    { NULL, NULL, NULL, 0 }
};

static const FlagMap ace_flags_map[] =
{
    { "OI", N_("ObjectInherit"),
      N_("Please add description"),
      SEC_ACE_FLAG_OBJECT_INHERIT },
    { "CI", N_("ContainerInherit"),
      N_("Please add description"),
      SEC_ACE_FLAG_CONTAINER_INHERIT },
    { "NP", N_("NoPropogateInherit"),
      N_("Please add description"),
      SEC_ACE_FLAG_NO_PROPAGATE_INHERIT },
    { "IO", N_("InheritOnly"),
      N_("Please add description"),
      SEC_ACE_FLAG_INHERIT_ONLY },
    { "ID", N_("InheritedACE"),
      N_("Please add description"),
      SEC_ACE_FLAG_INHERITED_ACE },
    { "SA", N_("SuccessfulAccess"),
      N_("Please add description"),
      SEC_ACE_FLAG_SUCCESSFUL_ACCESS },
    { "FA", N_("FailedAccess"),
      N_("Please add description"),
      SEC_ACE_FLAG_FAILED_ACCESS },

    { NULL, NULL, NULL, 0 }
};

static const FlagMap ace_access_mask_map[] =
{
    /* Keep masks with multiple bits set first */
    { "FA", N_("FileAll"),
      N_("Please add description"),
      SEC_RIGHTS_FILE_ALL },
    { "FR", N_("FileRead"),
      N_("Please add description"),
      SEC_RIGHTS_FILE_READ },
    { "FW", N_("FileWrite"),
      N_("Please add description"),
      SEC_RIGHTS_FILE_WRITE },
    { "FX", N_("FileExecuted"),
      N_("Please add description"),
      SEC_RIGHTS_FILE_EXECUTE },

    { "KA", N_("RegistryAll"),
      N_("Please add description"),
      SEC_RIGHTS_REG_ALL },
    { "KR", N_("RegistryRead"),
      N_("Please add description"),
      SEC_RIGHTS_REG_READ },
    { "KW", N_("RegistryWrite"),
      N_("Please add description"),
      SEC_RIGHTS_REG_WRITE },
    { "KX", N_("RegistryExecute"),
      N_("Please add description"),
      SEC_RIGHTS_REG_EXECUTE },

    { "GA", N_("GenericAll"),
      N_("Please add description"),
      SEC_GENERIC_ALL },
    { "GR", N_("GenericRead"),
      N_("Please add description"),
      SEC_GENERIC_READ },
    { "GW", N_("GenericWrite"),
      N_("Please add description"),
      SEC_GENERIC_WRITE },
    { "GX", N_("GenericExecute"),
      N_("Please add description"),
      SEC_GENERIC_EXECUTE },

    { "RC", N_("StdReadControl"),
      N_("Please add description"),
      SEC_STD_READ_CONTROL },
    { "SD", N_("StdDelete"),
      N_("Please add description"),
      SEC_STD_DELETE },
    { "WD", N_("StdWriteDAC"),
      N_("Please add description"),
      SEC_STD_WRITE_DAC },
    { "WO", N_("StdWriteOwner"),
      N_("Please add description"),
      SEC_STD_WRITE_OWNER },

    { "RP", N_("ADSReadProp"),
      N_("Please add description"),
      SEC_ADS_READ_PROP },
    { "WP", N_("ADSWriteProp"),
      N_("Please add description"),
      SEC_ADS_WRITE_PROP },
    { "CC", N_("ADSCreateChild"),
      N_("Please add description"),
      SEC_ADS_CREATE_CHILD },
    { "DC", N_("ADSDeleteChild"),
      N_("Please add description"),
      SEC_ADS_DELETE_CHILD },
    { "LC", N_("ADSList"),
      N_("Please add description"),
      SEC_ADS_LIST },
    { "SW", N_("ADSSelfWrite"),
      N_("Please add description"),
      SEC_ADS_SELF_WRITE },
    { "LO", N_("ADSListObject"),
      N_("Please add description"),
      SEC_ADS_LIST_OBJECT },
    { "DT", N_("ADSDeleteTree"),
      N_("Please add description"),
      SEC_ADS_DELETE_TREE },
    { "CR", N_("ADSControlAccess"),
      N_("Please add description"),
      SEC_ADS_CONTROL_ACCESS },

    { NULL, NULL, NULL, 0 }
};

ACEntry*
acentry_new(void)
{
    ACEntry *ret_val;

    ret_val = rra_new_type(ACEntry);
    ret_val->head.type = SEC_ACE_TYPE_INVALID;
    ret_val->head.flags = 0;
    ret_val->head.access_mask = 0;

    allocation_count++;

    return ret_val;
}

ACEntry*
acentry_new_parse_sddl_quick(const char *in_str)
{
    if (in_str == NULL)
    {
        return NULL;
    }

    return acentry_new_parse_sddl(&in_str);
}

ACEntry*
acentry_new_parse_sddl(const char **in_str)
{
    if (in_str == NULL || *in_str == NULL)
    {
        return NULL;
    }

    ACEntry *ret_val;

    char char_sep = ';';
    fm_flag_type type = 0;
    fm_flag_type flags = 0;
    fm_flag_type mask = 0;
    Sid *sid;

    /* parse ace type */
    if (!flag_map_names_to_flags(ace_types_map, in_str, &type, char_sep,
                                 TRUE))
    {
        rra_warning(N_("Unable to parse '%s' as type"), *in_str);
        return NULL;
    }
    if (**in_str != char_sep)
    {
        rra_warning(N_("Missing ; separator"));
        return NULL;
    }
    (*in_str)++;

    /* ace flags */
    if (!flag_map_names_to_flags(ace_flags_map, in_str, &flags, char_sep,
                                 TRUE))
    {
        rra_warning(N_("Unable to parse '%s' as flag"), *in_str);
        return NULL;
    }
    if (**in_str != char_sep)
    {
        rra_warning(N_("Missing ; separator after flags"));
        return NULL;
    }
    (*in_str)++;

    /* access mask */
    if (strncmp(*in_str, "0x", 2) == 0)
    {
        char *p;
        mask = strtol(*in_str, &p, 16);
        *in_str = p;
    }
    else
    {
        if (!flag_map_names_to_flags(ace_access_mask_map, in_str,
                                     &mask, char_sep, TRUE))
        {
            rra_warning(N_("Unable to parse '%s' as access mask"), *in_str);
            return NULL;
        }
    }
    if (**in_str != char_sep)
    {
        rra_warning(N_("Missing ; separator after access mask."));
        return NULL;
    }
    (*in_str)++;

    if (**in_str != char_sep)
    {
        rra_warning(N_("We don't parse guids"));
        return NULL;
    }
    (*in_str)++;

    if (**in_str != char_sep)
    {
        rra_warning(N_("We don't parse guids"));
        return NULL;
    }
    (*in_str)++;

    sid = sid_new_parse_sddl(in_str);
    if (sid == NULL)
    {
        rra_warning(N_("Unable to parse '%s' as sid"), *in_str);
        return NULL;
    }

    ret_val = acentry_new();
    acentry_set_type(ret_val, type);
    acentry_set_flags(ret_val, flags);
    acentry_set_access_mask(ret_val, mask);
    acentry_set_trustee(ret_val, sid);

    return ret_val;
}

static gboolean
is_object_type(acentry_type in_type)
{
    if (in_type == SEC_ACE_TYPE_ACCESS_ALLOWED_OBJECT
        || in_type == SEC_ACE_TYPE_ACCESS_DENIED_OBJECT
        || in_type == SEC_ACE_TYPE_SYSTEM_AUDIT_OBJECT
        || in_type == SEC_ACE_TYPE_SYSTEM_ALARM_OBJECT)
    {
        return TRUE;
    }
    return FALSE;
}

ACEntry*
acentry_new_parse_binary(const guint8 *in_data, gssize *in_size)
{
    int header_size = sizeof(struct acentry_header);

    if (in_data == NULL
        || in_size == NULL)
    {
        return NULL;
    }

    if (*in_size < header_size)
    {
        return NULL;
    }

    struct acentry_header *head = (struct acentry_header*)in_data;

    /** \todo Check type validity */
    if (is_object_type(head->type))
    {
        rra_warning(N_("Got unhandled ACEntry object type"));
    }

    /** \todo Check flags validity */

    if (*in_size < head->size)
    {
        return NULL;
    }

    /** \todo Check access mask validity */

    ACEntry *ret_val = acentry_new();

    ret_val->head.type = head->type;
    ret_val->head.flags = head->flags;
    ret_val->head.access_mask = head->access_mask;

    *in_size -= header_size;

    Sid *trustee = sid_new_parse_binary(in_data + header_size,
                                        in_size);

    if (trustee == NULL)
    {
        acentry_free(ret_val);
        return NULL;
    }

    ret_val->trustee = trustee;

    return ret_val;
}

ACEntry*
acentry_copy(const ACEntry *in_acentry)
{
    ACEntry *ret_val = acentry_new();

    acentry_set_type(ret_val, acentry_get_type(in_acentry));
    acentry_set_flags(ret_val, acentry_get_flags(in_acentry));
    acentry_set_access_mask(ret_val, acentry_get_access_mask(in_acentry));
    acentry_set_trustee(ret_val, sid_copy(acentry_get_trustee(in_acentry)));

    return ret_val;
}

gboolean
acentry_free(ACEntry *in_acentry)
{
    if (in_acentry == NULL)
    {
        return FALSE;
    }

    mark_dirty(in_acentry);

    if (in_acentry->trustee != NULL)
    {
        sid_free(in_acentry->trustee);
        in_acentry->trustee = NULL;
    }

    rra_free_type(ACEntry, in_acentry);
    allocation_count--;

    return TRUE;
}

acentry_type
acentry_get_type(const ACEntry *in_acentry)
{
    if (in_acentry == NULL)
    {
        return SEC_ACE_TYPE_INVALID;
    }

    return in_acentry->head.type;
}

gboolean
acentry_set_type(ACEntry *in_acentry, acentry_type in_type)
{
    if (in_acentry == NULL)
    {
        return FALSE;
    }

    in_acentry->head.type = in_type;
    mark_dirty(in_acentry);

    return TRUE;
}

acentry_flags
acentry_get_flags(const ACEntry *in_acentry)
{
    if (in_acentry == NULL)
    {
        return 0;
    }

    return in_acentry->head.flags;
}

gboolean
acentry_has_flag(const ACEntry *in_acentry, acentry_flags in_flags)
{
    if (in_acentry == NULL)
    {
        return FALSE;
    }

    return (in_acentry->head.flags & in_flags) == in_flags;
}

gboolean
acentry_set_flags(ACEntry *in_acentry, acentry_flags in_flags)
{
    if (in_acentry == NULL)
    {
        return FALSE;
    }

    in_acentry->head.flags = in_flags;

    mark_dirty(in_acentry);

    return TRUE;
}

gboolean
acentry_add_flag(ACEntry *in_acentry, acentry_flags in_flags)
{
    if (in_acentry == NULL)
    {
        return FALSE;
    }

    in_acentry->head.flags |= in_flags;

    mark_dirty(in_acentry);

    return TRUE;
}

gboolean
acentry_del_flag(ACEntry *in_acentry, acentry_flags in_flags)
{
    if (in_acentry == NULL)
    {
        return FALSE;
    }

    in_acentry->head.flags &= ~in_flags;

    mark_dirty(in_acentry);

    return TRUE;
}

acentry_access_mask
acentry_get_access_mask(const ACEntry *in_acentry)
{
    if (in_acentry == NULL)
    {
        return 0;
    }

    return in_acentry->head.access_mask;
}

gboolean
acentry_has_access_mask(const ACEntry *in_acentry, acentry_access_mask in_mask)
{
    if (in_acentry == NULL)
    {
        return FALSE;
    }

    return (in_acentry->head.access_mask & in_mask) == in_mask;
}

gboolean
acentry_set_access_mask(ACEntry *in_acentry, acentry_access_mask in_mask)
{
    if (in_acentry == NULL)
    {
        return FALSE;
    }

    in_acentry->head.access_mask = in_mask;

    mark_dirty(in_acentry);

    return TRUE;
}

gboolean
acentry_add_access_mask(ACEntry *in_acentry, acentry_access_mask in_mask)
{
    if (in_acentry == NULL)
    {
        return FALSE;
    }

    in_acentry->head.access_mask |= in_mask;

    mark_dirty(in_acentry);

    return TRUE;
}

gboolean
acentry_del_access_mask(ACEntry *in_acentry, acentry_access_mask in_mask)
{
    if (in_acentry == NULL)
    {
        return FALSE;
    }

    in_acentry->head.access_mask &= ~in_mask;

    mark_dirty(in_acentry);

    return TRUE;
}

const Sid*
acentry_get_trustee(const ACEntry *in_acentry)
{
    if (in_acentry == NULL)
    {
        return NULL;
    }

    return in_acentry->trustee;
}

gboolean
acentry_set_trustee(ACEntry *in_acentry, Sid *in_sid)
{
    if (in_acentry == NULL)
    {
        return FALSE;
    }

    in_acentry->trustee = in_sid;

    mark_dirty(in_acentry);

    return TRUE;
}

gboolean
acentry_equal(const ACEntry *ace1, const ACEntry *ace2)
{
    return acentry_compare(ace1, ace2) == 0;
}

int
acentry_compare(const ACEntry *ace1, const ACEntry *ace2)
{
    if (ace1 == ace2)
        return 0;
    if (!ace1)
        return -1;
    if (!ace2)
        return 1;

    if (ace1->head.type != ace2->head.type)
    {
        return ace1->head.type - ace2->head.type;
    }

    if (ace1->head.flags != ace2->head.flags)
    {
        return ace1->head.flags - ace2->head.flags;
    }

    if (ace1->head.access_mask != ace2->head.access_mask)
    {
        return ace1->head.access_mask - ace2->head.access_mask;
    }

    return sid_compare(ace1->trustee, ace2->trustee);
}

gboolean
acentry_write_to_binary(const ACEntry *in_acentry,
                        GByteArray *in_arr)
{
    if (in_acentry == NULL)
    {
        return FALSE;
    }

    if (in_acentry->trustee == NULL)
    {
        return FALSE;
    }

    int before_len = in_arr->len;

    g_byte_array_append(in_arr, (const guint8*)&in_acentry->head,
                        sizeof(struct acentry_header));

    if (!sid_write_to_binary(in_acentry->trustee, in_arr))
    {
        return FALSE;
    }

    struct acentry_header *header =
        (struct acentry_header*)(in_arr->data + before_len);
    header->size = in_arr->len - before_len;

    return TRUE;
}

GByteArray*
acentry_to_binary(const ACEntry *in_acentry)
{
    GByteArray *ret_val = g_byte_array_new();

    if (!acentry_write_to_binary(in_acentry, ret_val))
    {
        g_byte_array_free(ret_val, TRUE);
        return NULL;
    }

    return ret_val;
}

const char*
acentry_to_sddl(const ACEntry *in_acentry)
{
    if (in_acentry == NULL)
    {
        return NULL;
    }

    if (acentry_get_trustee(in_acentry) == NULL)
    {
        return NULL;
    }

    if (in_acentry->sddl == NULL)
    {
        fm_flag_type flags;
        const char *s_trustee;

        ((ACEntry*)in_acentry)->sddl = ustr_new();

        flags = acentry_get_type(in_acentry);
        if (!flag_map_flags_to_names(ace_types_map,
                                     &flags,
                                     in_acentry->sddl,
                                     TRUE))
        {
            rra_warning(N_("Unable to generate type str from %#x"),
                        acentry_get_type(in_acentry));
            mark_dirty((ACEntry*)in_acentry);
            return NULL;
        }
        ustr_printfa(in_acentry->sddl, ";");

        flags = acentry_get_flags(in_acentry);
        if (!flag_map_flags_to_names(ace_flags_map,
                                     &flags,
                                     in_acentry->sddl,
                                     TRUE))
        {
            rra_warning(N_("Unable to generate flags str from %#x"),
                        acentry_get_flags(in_acentry));
            mark_dirty((ACEntry*)in_acentry);
            return NULL;
        }
        ustr_printfa(in_acentry->sddl, ";");

        flags = acentry_get_access_mask(in_acentry);
        if (!flag_map_flags_to_names(ace_access_mask_map,
                                     &flags,
                                     in_acentry->sddl,
                                     TRUE))
        {
            ustr_printfa(in_acentry->sddl, "%#08x",
                         acentry_get_access_mask(in_acentry));
        }
        ustr_printfa(in_acentry->sddl, ";");

        ustr_printfa(in_acentry->sddl, ";");

        ustr_printfa(in_acentry->sddl, ";");

        s_trustee = sid_to_sddl(acentry_get_trustee(in_acentry));
        ustr_printfa(in_acentry->sddl, "%s", s_trustee);
    }

    return ustr_as_utf8(in_acentry->sddl);
}
