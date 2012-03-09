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
#include <stdio.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/secdesc/security_descriptor.h>
#include <rregadmin/secdesc/acentry.h>
#include <rregadmin/secdesc/aclist.h>
#include <rregadmin/util/flag_map.h>
#include <rregadmin/secdesc/sid.h>
#include <rregadmin/secdesc/log.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/malloc.h>

struct secdesc_binary
{
    secdesc_rev rev;
    secdesc_type type;
    secdesc_offset owner;
    secdesc_offset group;
    secdesc_offset sacl;
    secdesc_offset dacl;
} RRA_VERBATIM_STRUCT;

struct SecurityDescriptor_
{
    secdesc_rev rev;
    secdesc_type type;
    secdesc_type orig_type;
    Sid *owner;
    Sid *group;
    ACList *sacl;
    ACList *dacl;

    ustring *sddl; /* mutable */
};

static void
mark_dirty(SecurityDescriptor *in_sd)
{
    if (in_sd->sddl != NULL)
    {
        ustr_free(in_sd->sddl);
        in_sd->sddl = NULL;
    }
}

static guint32 allocation_count = 0;

guint32
secdesc_allocation_count(void)
{
    return allocation_count;
}

static const FlagMap dacl_flags[] =
{
    { "P",  N_("Protected"), N_("Please add description"),
      SEC_DESC_DACL_PROTECTED },
    { "AR", N_("AutoInheritReq"),N_("Please add description"),
       SEC_DESC_DACL_AUTO_INHERIT_REQ },
    { "AI", N_("AutoInherited"), N_("Please add description"),
      SEC_DESC_DACL_AUTO_INHERITED },

    { NULL, NULL, NULL, 0 }
};

static const FlagMap sacl_flags[] =
{
    { "P",  N_("Protected"), N_("Please add description"),
      SEC_DESC_SACL_PROTECTED },
    { "AR", N_("AutoInheritReq"), N_("Please add description"),
      SEC_DESC_SACL_AUTO_INHERIT_REQ },
    { "AI", N_("AutoInherited"), N_("Please add description"),
      SEC_DESC_SACL_AUTO_INHERITED },

    { NULL, NULL, NULL, 0 }
};

static SecurityDescriptor*
secdesc_new_bare(void)
{
    SecurityDescriptor *ret_val;

    ret_val = rra_new_type(SecurityDescriptor);

    ret_val->rev = SECDESC_DEFAULT_REV;
    ret_val->type = SECDESC_DEFAULT_TYPE;
    ret_val->orig_type = SECDESC_ERROR_TYPE;
    allocation_count++;

    return ret_val;
}

SecurityDescriptor*
secdesc_new(void)
{
    SecurityDescriptor *ret_val = secdesc_new_bare();

    ret_val->sacl = aclist_new();
    ret_val->dacl = aclist_new();

    return ret_val;
}

SecurityDescriptor*
secdesc_new_full(Sid *in_owner, Sid *in_group)
{
    SecurityDescriptor *ret_val = secdesc_new();
    ret_val->owner = in_owner;
    ret_val->group = in_group;
    return ret_val;
}

SecurityDescriptor*
secdesc_new_parse_sddl_quick(const char *in_str)
{
    if (in_str == NULL)
    {
        return NULL;
    }

    return secdesc_new_parse_sddl(&in_str);
}

SecurityDescriptor*
secdesc_new_parse_sddl(const char **in_str)
{
    if (in_str == NULL || *in_str == NULL)
    {
        return NULL;
    }

    gboolean failed = FALSE;
    SecurityDescriptor *ret_val = secdesc_new_bare();

    while (*in_str[0] != '\0')
    {
        fm_flag_type flags;
        char c = (*in_str)[0];
        if ((*in_str)[1] != ':')
        {
            rra_warning(N_("No : after %c code: %s"), c, *in_str);
            failed = TRUE;
            break;
        }
        *in_str += 2;

        switch (c)
        {
        case 'D':
            if (ret_val->dacl != NULL)
            {
                rra_warning(N_("Already got dacl section"));
                failed = TRUE;
            }
            else
            {
                flags = 0;
                if (!flag_map_names_to_flags(dacl_flags, in_str, &flags, '(',
                                             TRUE))
                {
                    rra_warning(N_("Failed to parse dacl flags: %s"), *in_str);
                    failed = TRUE;
                }
                else
                {
                    ret_val->type |= flags | SEC_DESC_DACL_PRESENT;
                    ret_val->dacl = aclist_new_parse_sddl(in_str);
                    if (ret_val->dacl == NULL)
                    {
                        rra_warning(N_("Failed to parse dacl acl: %s"),
                                    *in_str);
                        failed = TRUE;
                    }
                }
            }
            break;
        case 'S':
            if (ret_val->sacl != NULL)
            {
                rra_warning(N_("Already got sacl section"));
                failed = TRUE;
            }
            else
            {
                flags = 0;
                if (!flag_map_names_to_flags(sacl_flags, in_str, &flags, '(',
                                             TRUE))
                {
                    rra_warning(N_("Failed to parse sacl flags: %s"), *in_str);
                    failed = TRUE;
                }
                else
                {
                    ret_val->type |= flags | SEC_DESC_SACL_PRESENT;
                    ret_val->sacl = aclist_new_parse_sddl(in_str);
                    if (ret_val->sacl == NULL)
                    {
                        rra_warning(N_("Failed to parse sacl acl: %s"),
                                    *in_str);
                        failed = TRUE;
                    }
                }
            }
            break;
        case 'O':
            if (ret_val->owner != NULL)
            {
                rra_warning(N_("Already got owner section"));
                failed = TRUE;
            }
            else
            {
                ret_val->owner = sid_new_parse_sddl(in_str);
                if (ret_val->owner == NULL)
                {
                    rra_warning(N_("Failed to parse owner sid: %s"), *in_str);
                    failed = TRUE;
                }
            }
            break;
        case 'G':
            if (ret_val->group != NULL)
            {
                rra_warning(N_("Already got group section"));
                failed = TRUE;
            }
            else
            {
                ret_val->group = sid_new_parse_sddl(in_str);
                if (ret_val->group == NULL)
                {
                    rra_warning(N_("Failed to parse group sid: %s"), *in_str);
                    failed = TRUE;
                }
            }
            break;
        default:
            rra_warning(N_("Code char %c invalid"), c);
            failed = TRUE;
            break;
        }

        if (failed)
            break;
    }

    if (failed)
    {
        secdesc_free(ret_val);
        return NULL;
    }

    if (ret_val->dacl == NULL)
    {
        ret_val->dacl = aclist_new();
    }

    if (ret_val->sacl == NULL)
    {
        ret_val->sacl = aclist_new();
    }

    return ret_val;
}

SecurityDescriptor*
secdesc_new_parse_binary(const guint8 *in_data, gsize *in_size)
{
    if (in_data == NULL || in_size == NULL)
    {
        return NULL;
    }

    if (*in_size < sizeof(struct secdesc_binary))
    {
        return NULL;
    }

    struct secdesc_binary *b = (struct secdesc_binary*)in_data;

    if ((b->type & SEC_DESC_SELF_RELATIVE) == 0)
    {
        rra_warning(N_("Unable to handle non self relative "
                      "SecurityDescriptors"));
        return NULL;
    }

    gboolean failed = FALSE;
    int largest_offset = 0;
    Sid *owner = NULL;
    Sid *group = NULL;
    ACList *dacl = NULL;
    ACList *sacl = NULL;

    if (b->owner > 0)
    {
        gssize allowed_size = *in_size - b->owner;
        owner = sid_new_parse_binary(in_data + b->owner, &allowed_size);
        if (owner == NULL)
        {
            rra_warning(N_("Unable to parse owner sid"));
            failed = TRUE;
        }
        else if ((*in_size - allowed_size) > largest_offset)
        {
            largest_offset = *in_size - allowed_size;
        }
    }
    if (failed)
    {
        return NULL;
    }

    if (b->group > 0)
    {
        gssize allowed_size = *in_size - b->group;
        group = sid_new_parse_binary(in_data + b->group, &allowed_size);
        if (group == NULL)
        {
            rra_warning(N_("Unable to parse group sid"));
            failed = TRUE;
        }
        else if ((*in_size - allowed_size) > largest_offset)
        {
            largest_offset = *in_size - allowed_size;
        }
    }
    if (failed)
    {
        return NULL;
    }

    int removed_flags = (SEC_DESC_SELF_RELATIVE
                         | SEC_DESC_DACL_PRESENT
                         | SEC_DESC_SACL_PRESENT);

    SecurityDescriptor *ret_val = secdesc_new_bare();
    ret_val->rev = b->rev;
    ret_val->type = b->type & ~removed_flags;
    ret_val->orig_type = b->type;

    if (((b->type & SEC_DESC_DACL_PRESENT) > 0)
        && (b->dacl > 0))
    {
        gssize allowed_size = *in_size - b->dacl;
        dacl = aclist_new_parse_binary(in_data + b->dacl, &allowed_size);
        if (dacl == NULL)
        {
            rra_warning(N_("Unable to parse dacl aclist"));
            failed = TRUE;
        }
        else if ((*in_size - allowed_size) > largest_offset)
        {
            largest_offset = *in_size - allowed_size;
        }
    }
    else
    {
        // If no DACL present remove any extra dacl flags.
        ret_val->type = (ret_val->type & ~DACL_EXTRA_CODES);
    }

    if (failed)
    {
        secdesc_free(ret_val);
        return NULL;
    }

    if (((b->type & SEC_DESC_SACL_PRESENT) > 0)
        && (b->sacl > 0))
    {
        gssize allowed_size = *in_size - b->sacl;
        sacl = aclist_new_parse_binary(in_data + b->sacl, &allowed_size);
        if (sacl == NULL)
        {
            rra_warning(N_("Unable to parse sacl aclist"));
            failed = TRUE;
        }
        else if ((*in_size - allowed_size) > largest_offset)
        {
            largest_offset = *in_size - allowed_size;
        }
    }
    else
    {
        // If no SACL present remove any extra sacl flags.
        ret_val->type = (ret_val->type & ~SACL_EXTRA_CODES);
    }

    if (failed)
    {
        secdesc_free(ret_val);
        return NULL;
    }

    ret_val->owner = owner;
    ret_val->group = group;
    if (dacl == NULL)
    {
        ret_val->dacl = aclist_new();
    }
    else
    {
        ret_val->dacl = dacl;
    }
    if (sacl == NULL)
    {
        ret_val->sacl = aclist_new();
    }
    else
    {
        ret_val->sacl = sacl;
    }

    *in_size -= largest_offset;

    return ret_val;
}

SecurityDescriptor*
secdesc_copy(const SecurityDescriptor *in_sd)
{
    if (in_sd == NULL)
    {
        return NULL;
    }
    SecurityDescriptor *ret_val = secdesc_new_bare();

    ret_val->rev = in_sd->rev;
    ret_val->type = in_sd->type;
    ret_val->owner = sid_copy(in_sd->owner);
    ret_val->group = sid_copy(in_sd->group);
    ret_val->dacl = aclist_copy(in_sd->dacl);
    ret_val->sacl = aclist_copy(in_sd->sacl);

    return ret_val;
}

gboolean
secdesc_free(SecurityDescriptor *in_sd)
{
    if (in_sd == NULL)
    {
        return FALSE;
    }

    aclist_free(in_sd->sacl);
    in_sd->sacl = NULL;

    aclist_free(in_sd->dacl);
    in_sd->dacl = NULL;

    if (in_sd->owner != NULL)
    {
        sid_free(in_sd->owner);
        in_sd->owner = NULL;
    }

    if (in_sd->group != NULL)
    {
        sid_free(in_sd->group);
        in_sd->group = NULL;
    }

    mark_dirty(in_sd);

    rra_free_type(SecurityDescriptor, in_sd);
    allocation_count--;

    return TRUE;
}

secdesc_rev
secdesc_get_rev(const SecurityDescriptor *in_sd)
{
    if (in_sd == NULL)
    {
        return SECDESC_ERROR_REV;
    }

    return in_sd->rev;
}

gboolean
secdesc_set_rev(SecurityDescriptor *in_sd, secdesc_rev in_rev)
{
    if (in_sd == NULL)
    {
        return FALSE;
    }

    in_sd->rev = in_rev;
    mark_dirty(in_sd);
    return TRUE;
}

secdesc_type
secdesc_get_type(const SecurityDescriptor *in_sd)
{
    if (in_sd == NULL)
    {
        return SECDESC_ERROR_TYPE;
    }

    secdesc_type ret_val = in_sd->type;
    if (secdesc_has_type_code(in_sd, SEC_DESC_DACL_PRESENT))
    {
        ret_val |= SEC_DESC_DACL_PRESENT;
    }
    if (secdesc_has_type_code(in_sd, SEC_DESC_SACL_PRESENT))
    {
        ret_val |= SEC_DESC_SACL_PRESENT;
    }

    ret_val |= SEC_DESC_SELF_RELATIVE;

    return ret_val;
}

gboolean
secdesc_has_type_code(const SecurityDescriptor *in_sd,
                      secdesc_type_code in_code)
{
    switch (in_code)
    {
    case SEC_DESC_SELF_RELATIVE:
        return TRUE;

    case SEC_DESC_DACL_PRESENT:
        return aclist_get_acentry_count(in_sd->dacl) > 0;

    case SEC_DESC_SACL_PRESENT:
        return aclist_get_acentry_count(in_sd->sacl) > 0;

    default:
        return (in_sd->type & in_code) > 0;
    }
}

gboolean
secdesc_set_type(SecurityDescriptor *in_sd, secdesc_type in_type)
{
    if (in_sd == NULL)
    {
        return FALSE;
    }

    in_sd->type = in_type;
    mark_dirty(in_sd);
    return TRUE;
}

gboolean
secdesc_set_type_code(SecurityDescriptor *in_sd, secdesc_type_code in_code)
{
    switch (in_code)
    {
    case SEC_DESC_SELF_RELATIVE:
    case SEC_DESC_DACL_PRESENT:
    case SEC_DESC_SACL_PRESENT:
        return FALSE;

    default:
        if (secdesc_has_type_code(in_sd, in_code))
        {
            return FALSE;
        }
        in_sd->type |= in_code;
        return TRUE;
    }
}

gboolean
secdesc_unset_type_code(SecurityDescriptor *in_sd, secdesc_type_code in_code)
{
    switch (in_code)
    {
    case SEC_DESC_SELF_RELATIVE:
    case SEC_DESC_DACL_PRESENT:
    case SEC_DESC_SACL_PRESENT:
        return FALSE;

    default:
        if (!secdesc_has_type_code(in_sd, in_code))
        {
            return FALSE;
        }
        in_sd->type &= ~in_code;
        return TRUE;
    }
}

const Sid*
secdesc_get_owner(const SecurityDescriptor *in_sd)
{
    if (in_sd == NULL)
    {
        return NULL;
    }

    return in_sd->owner;
}

gboolean
secdesc_set_owner(SecurityDescriptor *in_sd, Sid *in_owner)
{
    if (in_sd == NULL || in_owner == NULL)
    {
        return FALSE;
    }

    in_sd->owner = in_owner;
    mark_dirty(in_sd);

    return TRUE;
}

const Sid*
secdesc_get_group(const SecurityDescriptor *in_sd)
{
    if (in_sd == NULL)
    {
        return NULL;
    }

    return in_sd->group;
}

gboolean
secdesc_set_group(SecurityDescriptor *in_sd, Sid *in_group)
{
    if (in_sd == NULL || in_group == NULL)
    {
        return FALSE;
    }

    in_sd->group = in_group;
    mark_dirty(in_sd);

    return TRUE;
}

ACList*
secdesc_get_sacl(SecurityDescriptor *in_sd)
{
    if (in_sd == NULL)
    {
        return NULL;
    }

    return in_sd->sacl;
}

ACList*
secdesc_get_dacl(SecurityDescriptor *in_sd)
{
    if (in_sd == NULL)
    {
        return NULL;
    }

    return in_sd->dacl;
}

int
secdesc_compare(const SecurityDescriptor *in_sd1,
                const SecurityDescriptor *in_sd2)
{
    if (in_sd1 == in_sd2)
        return 0;
    if (!in_sd1)
        return -1;
    if (!in_sd2)
        return 1;

    if (in_sd1->rev != in_sd2->rev)
    {
        rra_debug (N_("Revs differ %d vs %d"),
                   in_sd1->rev, in_sd2->rev);
        return in_sd1->rev - in_sd2->rev;
    }
    secdesc_type typ1 = secdesc_get_type(in_sd1);
    secdesc_type typ2 = secdesc_get_type(in_sd2);
    if (typ1 != typ2)
    {
        ustring *typ1_str = secdesc_type_to_ustr(typ1);
        ustring *typ2_str = secdesc_type_to_ustr(typ2);

        rra_debug (N_("Types differ %s(0x%04X) vs %s(0x%04X)"),
                   ustr_as_utf8(typ1_str), typ1,
                   ustr_as_utf8(typ2_str), typ2);

        ustr_free(typ1_str);
        ustr_free(typ2_str);
        return typ1 - typ2;
    }

    int ret_val;

    ret_val = sid_compare(secdesc_get_owner(in_sd1),
                          secdesc_get_owner(in_sd2));
    if (ret_val != 0)
    {
        rra_debug (N_("Owner sids differ"));
        return ret_val;
    }

    ret_val = sid_compare(secdesc_get_group(in_sd1),
                          secdesc_get_group(in_sd2));
    if (ret_val != 0)
    {
        rra_debug (N_("Group sids differ"));
        return ret_val;
    }

    ret_val = aclist_compare(secdesc_get_dacl((SecurityDescriptor*)in_sd1),
                             secdesc_get_dacl((SecurityDescriptor*)in_sd2));
    if (ret_val != 0)
    {
        rra_debug (N_("DACL lists differ"));
        return ret_val;
    }

    ret_val = aclist_compare(secdesc_get_sacl((SecurityDescriptor*)in_sd1),
                             secdesc_get_sacl((SecurityDescriptor*)in_sd2));
    if (ret_val != 0)
    {
        rra_debug (N_("SACL lists differ"));
        return ret_val;
    }

    return 0;
}

gboolean
secdesc_equal(const SecurityDescriptor *in_sd1,
              const SecurityDescriptor *in_sd2)
{
    return secdesc_compare(in_sd1, in_sd2) == 0;
}

GByteArray*
secdesc_to_binary(const SecurityDescriptor *in_sd, int in_block_size)
{
    if (in_sd == NULL)
    {
        return NULL;
    }
    GByteArray *ret_val = g_byte_array_new();
    if (!secdesc_write_to_binary(in_sd, ret_val, in_block_size))
    {
        g_byte_array_free(ret_val, TRUE);
        return NULL;
    }
    return ret_val;
}

gboolean
secdesc_write_to_binary(const SecurityDescriptor *in_sd, GByteArray *in_arr,
                        int in_block_size)
{
    if (in_sd == NULL || in_arr == NULL)
    {
        return FALSE;
    }

    int next_offset;
    int initial_offset = in_arr->len;
    int type = in_sd->type;
    type |= SEC_DESC_SELF_RELATIVE;
    if (secdesc_has_type_code(in_sd, SEC_DESC_DACL_PRESENT))
    {
        type |= SEC_DESC_DACL_PRESENT;
    }
    if (secdesc_has_type_code(in_sd, SEC_DESC_SACL_PRESENT))
    {
        type |= SEC_DESC_SACL_PRESENT;
    }
    type |= SEC_DESC_SELF_RELATIVE;

    if (in_sd->orig_type != SECDESC_ERROR_TYPE
        && type != in_sd->orig_type)
    {
        rra_warning(
            N_("Generated type, 0x%04X, differs from original, 0x%04X."
               "  Using the original type."),
            type, in_sd->orig_type);
        type = in_sd->orig_type;
    }

    struct secdesc_binary tmpb;
    tmpb.rev = in_sd->rev;
    tmpb.type = type;
    tmpb.owner = 0;
    tmpb.group = 0;
    tmpb.dacl = 0;
    tmpb.sacl = 0;

    g_byte_array_append(in_arr, (guint8*)&tmpb, sizeof(struct secdesc_binary));

    next_offset = in_arr->len - initial_offset;
    if (secdesc_has_type_code(in_sd, SEC_DESC_DACL_PRESENT))
    {
        if (!aclist_write_to_binary(in_sd->dacl, in_arr, in_block_size))
        {
            return FALSE;
        }
        ((struct secdesc_binary*)(in_arr->data + initial_offset))->dacl =
            next_offset;
    }

    next_offset = in_arr->len - initial_offset;
    if (secdesc_has_type_code(in_sd, SEC_DESC_SACL_PRESENT))
    {
        if (!aclist_write_to_binary(in_sd->sacl, in_arr, in_block_size))
        {
            return FALSE;
        }
        ((struct secdesc_binary*)(in_arr->data + initial_offset))->sacl =
            next_offset;
    }

    next_offset = in_arr->len - initial_offset;
    if (in_sd->owner != NULL)
    {
        if (!sid_write_to_binary(in_sd->owner, in_arr))
        {
            return FALSE;
        }
        ((struct secdesc_binary*)(in_arr->data + initial_offset))->owner =
            next_offset;
    }

    next_offset = in_arr->len - initial_offset;
    if (in_sd->group != NULL)
    {
        if (!sid_write_to_binary(in_sd->group, in_arr))
        {
            return FALSE;
        }
        ((struct secdesc_binary*)(in_arr->data + initial_offset))->group =
            next_offset;
    }

    return TRUE;
}

const char*
secdesc_to_sddl(const SecurityDescriptor *in_sd)
{
    if (in_sd == NULL)
    {
        return NULL;
    }
    if (in_sd->sddl == NULL)
    {
        ((SecurityDescriptor*)in_sd)->sddl = ustr_new();

        if (in_sd->owner != NULL)
        {
            ustr_printfa(in_sd->sddl, "O:");
            ustr_printfa(in_sd->sddl, "%s", sid_to_sddl(in_sd->owner));
        }

        if (in_sd->group != NULL)
        {
            ustr_printfa(in_sd->sddl, "G:");
            ustr_printfa(in_sd->sddl, "%s", sid_to_sddl(in_sd->group));
        }

        if (secdesc_has_type_code(in_sd, SEC_DESC_DACL_PRESENT))
        {
            fm_flag_type flags = in_sd->type;
            ustr_printfa(in_sd->sddl, "D:");
            flag_map_flags_to_names(dacl_flags, &flags, in_sd->sddl,
                                    FALSE);
            ustr_printfa(in_sd->sddl, "%s", aclist_to_sddl(in_sd->dacl));
        }

        if (secdesc_has_type_code(in_sd, SEC_DESC_SACL_PRESENT))
        {
            fm_flag_type flags = in_sd->type;
            ustr_printfa(in_sd->sddl, "S:");
            flag_map_flags_to_names(sacl_flags, &flags, in_sd->sddl,
                                    FALSE);
            ustr_printfa(in_sd->sddl, "%s", aclist_to_sddl(in_sd->sacl));
        }
    }

    return ustr_as_utf8(in_sd->sddl);
}
