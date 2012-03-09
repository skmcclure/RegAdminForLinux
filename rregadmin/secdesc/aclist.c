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

#include <stdio.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/secdesc/aclist.h>
#include <rregadmin/secdesc/acentry.h>
#include <rregadmin/secdesc/log.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/malloc.h>

struct aclist_header
{
    aclist_rev rev;
    guint16 size;
    aclist_acentry_count num_aces;
} RRA_VERBATIM_STRUCT;

struct aclist_binary
{
    struct aclist_header head;
    guint8 data[];
} RRA_VERBATIM_STRUCT;

struct ACList_
{
    aclist_rev rev;
    ustring *sddl;
    GPtrArray *entries;
};


static void
mark_dirty(ACList *in_list)
{
    if (in_list->sddl != NULL)
    {
        ustr_free(in_list->sddl);
        in_list->sddl = NULL;
    }
}

static guint32 allocation_count = 0;

guint32
aclist_allocation_count(void)
{
    return allocation_count;
}

ACList*
aclist_new(void)
{
    ACList *ret_val = rra_new_type(ACList);
    ret_val->rev = ACLIST_DEFAULT_REV;
    ret_val->entries = g_ptr_array_new();
    allocation_count++;
    return ret_val;
}

ACList*
aclist_new_parse_sddl_quick(const char *in_str)
{
    if (in_str == NULL)
    {
        return NULL;
    }

    return aclist_new_parse_sddl(&in_str);
}

ACList*
aclist_new_parse_sddl(const char **in_str)
{
    if (in_str == NULL || *in_str == NULL)
    {
        return NULL;
    }

    if (*in_str[0] != '(')
    {
        rra_warning("No ( to start the aclist");
        return NULL;
    }

    ACList *ret_val = aclist_new();

    while (*in_str[0] == '(')
    {
        (*in_str)++;
        ACEntry *entry = acentry_new_parse_sddl(in_str);
        if (entry == NULL)
        {
            aclist_free(ret_val);
            return NULL;
        }
        else
        {
            aclist_push_acentry(ret_val, entry);
        }

        if (*in_str[0] != ')')
        {
            rra_warning("No ) to end acentry");
            aclist_free(ret_val);
            return NULL;
        }
        (*in_str)++;
    }

    return ret_val;
}

ACList*
aclist_new_parse_binary(const guint8 *in_data, gssize *in_size)
{
    if (in_data == NULL
        || in_size == NULL)
    {
        return NULL;
    }

    if (*in_size < sizeof(struct aclist_header))
    {
        return NULL;
    }

    struct aclist_header *header = (struct aclist_header*)in_data;

    if (*in_size < header->size)
    {
        return NULL;
    }
    *in_size -= header->size;

    gssize allow_size = header->size - sizeof(struct aclist_header);
    ACList *ret_val = aclist_new();
    aclist_set_rev(ret_val, header->rev);

    int i;
    for (i = 0; i < header->num_aces; i++)
    {
        ACEntry *entry =
            acentry_new_parse_binary(in_data + header->size - allow_size,
                                     &allow_size);
        aclist_push_acentry(ret_val, entry);
    }

    return ret_val;
}

ACList*
aclist_copy(const ACList *in_aclist)
{
    if (in_aclist == NULL)
    {
        return NULL;
    }
    int i;
    ACList *ret_val = aclist_new();

    ret_val->rev = in_aclist->rev;

    for (i = 0; i < in_aclist->entries->len; i++)
    {
        aclist_push_acentry(
            ret_val, acentry_copy(aclist_get_acentry(in_aclist, i)));
    }

    return ret_val;
}

gboolean
aclist_free(ACList *in_aclist)
{
    if (in_aclist == NULL)
    {
        return FALSE;
    }
    gboolean ret_val = TRUE;

    while (in_aclist->entries->len > 0)
    {
        ACEntry *entry =
            (ACEntry*)g_ptr_array_remove_index_fast(in_aclist->entries, 0);
        if (!acentry_free(entry))
        {
            ret_val = FALSE;
        }
    }

    g_ptr_array_free(in_aclist->entries, TRUE);
    in_aclist->entries = NULL;

    mark_dirty(in_aclist);

    rra_free_type(ACList, in_aclist);
    allocation_count--;

    return ret_val;
}

aclist_rev
aclist_get_rev(const ACList *in_aclist)
{
    if (in_aclist == NULL)
    {
        return ACLIST_ERROR_REV;
    }

    return in_aclist->rev;
}

gboolean
aclist_set_rev(ACList *in_aclist, aclist_rev in_rev)
{
    if (in_aclist == NULL)
    {
        return FALSE;
    }

    mark_dirty(in_aclist);

    in_aclist->rev = in_rev;

    return TRUE;
}

aclist_acentry_count
aclist_get_acentry_count(const ACList *in_aclist)
{
    if (in_aclist == NULL)
    {
        return ACLIST_ERROR_ACENTRY_COUNT;
    }

    return in_aclist->entries->len;
}

gboolean
aclist_push_acentry(ACList *in_aclist, ACEntry *in_entry)
{
    if (in_aclist == NULL
        || in_entry == NULL)
    {
        return FALSE;
    }

    mark_dirty(in_aclist);

    g_ptr_array_add(in_aclist->entries, in_entry);

    return TRUE;
}

gboolean
aclist_pop_acentry(ACList *in_aclist)
{
    if (in_aclist == NULL)
    {
        return FALSE;
    }

    if (in_aclist->entries->len == 0)
    {
        return FALSE;
    }

    int rm_index = in_aclist->entries->len - 1;

    mark_dirty(in_aclist);

    ACEntry *entry = (ACEntry*)g_ptr_array_index(in_aclist->entries, rm_index);
    g_ptr_array_remove_index(in_aclist->entries, rm_index);

    acentry_free(entry);

    return TRUE;
}

const ACEntry*
aclist_get_acentry(const ACList *in_aclist, gint8 in_index)
{
    if (in_aclist == NULL)
    {
        return NULL;
    }

    if (in_index >= in_aclist->entries->len
        || in_index < 0)
    {
        return NULL;
    }

    return (ACEntry*)in_aclist->entries->pdata[in_index];
}

int
aclist_compare(const ACList *in_aclist1, const ACList *in_aclist2)
{
    int i;

    if (in_aclist1 == in_aclist2)
        return 0;
    if (!in_aclist1)
        return -1;
    if (!in_aclist2)
        return 1;

    if (in_aclist1->rev != in_aclist2->rev)
    {
        rra_debug(N_("ACList revs differ: %d vs %d"),
                  in_aclist1->rev,
                  in_aclist2->rev);
        return in_aclist1->rev - in_aclist2->rev;
    }

    if (aclist_get_acentry_count(in_aclist1)
        != aclist_get_acentry_count(in_aclist2))
    {
        rra_debug(N_("ACList counts differ: %d vs %d"),
                  aclist_get_acentry_count(in_aclist1),
                  aclist_get_acentry_count(in_aclist2));
        return (aclist_get_acentry_count(in_aclist1)
                - aclist_get_acentry_count(in_aclist2));
    }

    for (i = 0; i < aclist_get_acentry_count(in_aclist1); i++)
    {
        int ret_val = acentry_compare(aclist_get_acentry(in_aclist1, i),
                                      aclist_get_acentry(in_aclist2, i));
        if (ret_val != 0)
        {
            rra_debug(N_("ACList entries %d differ %s vs %s"),
                      i,
                      acentry_to_sddl(aclist_get_acentry(in_aclist1, i)),
                      acentry_to_sddl(aclist_get_acentry(in_aclist2, i)));
            return ret_val;
        }
    }

    return 0;
}

gboolean
aclist_equal(const ACList *aclist1, const ACList *aclist2)
{
    return aclist_compare(aclist1, aclist2) == 0;
}

GByteArray*
aclist_to_binary(const ACList *in_aclist, gssize in_block_size)
{
    if (in_aclist == NULL)
    {
        return NULL;
    }

    GByteArray *ret_val = g_byte_array_new();

    if (!aclist_write_to_binary(in_aclist, ret_val, in_block_size))
    {
        g_byte_array_free(ret_val, TRUE);
        return NULL;
    }

    return ret_val;
}

gboolean
aclist_write_to_binary(const ACList *in_aclist, GByteArray *in_arr,
                       gssize in_block_size)
{
    if (in_aclist == NULL
        || in_arr == NULL)
    {
        return FALSE;
    }

    int i;
    int amount_written;
    int start_size = in_arr->len;
    struct aclist_header header;
    header.rev = in_aclist->rev;
    header.size = 0;
    header.num_aces = aclist_get_acentry_count(in_aclist);

    g_byte_array_append(in_arr, (guint8*)&header,
                        sizeof(struct aclist_header));

    for (i = 0; i < header.num_aces; i++)
    {
        const ACEntry *entry = aclist_get_acentry(in_aclist, i);
        if (!acentry_write_to_binary(entry, in_arr))
        {
            return FALSE;
        }
    }

    amount_written = in_arr->len - start_size;

    if (in_block_size > 0)
    {
        int i;
        guint8 wdata[] = { 0x00 };
        int left_to_write = (in_block_size - amount_written) % in_block_size;
        for (i = 0; i < left_to_write; i++)
        {
            g_byte_array_append(in_arr, wdata, 1);
        }
        amount_written += left_to_write;
    }

    struct aclist_header *headerp =
        (struct aclist_header*)(in_arr->data + start_size);
    headerp->size = amount_written;

    return TRUE;
}

const char*
aclist_to_sddl(const ACList *in_aclist)
{
    if (in_aclist == NULL)
    {
        return NULL;
    }

    if (in_aclist->sddl == NULL)
    {
        int i;
        ((ACList*)in_aclist)->sddl = ustr_new();

        for (i = 0; i < aclist_get_acentry_count(in_aclist); i++)
        {
            ustr_printfa(in_aclist->sddl, "(%s)",
                         acentry_to_sddl(aclist_get_acentry(in_aclist, i)));
        }
    }

    return ustr_as_utf8(in_aclist->sddl);
}
