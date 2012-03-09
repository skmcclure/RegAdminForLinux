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

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/hive/security_descriptor_cell.h>
#include <rregadmin/hive/defines.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/util/check.h>
#include <rregadmin/hive/hcheck_decl.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/hive/log.h>
#include <rregadmin/hive/hive.h>

/** Security descriptor.
 *
 * @internal
 *
 * @ingroup sdc_hive_group
 */
struct SecurityDescriptorCellData
{
    /* 0x0000   Word    ID: ASCII-"sk" = 0x6B73        */
    struct CellHeader cell_head;
    /* 0x0002   Word    Unused                         */
    int16_t dummy1;
    /* 0x0004   D-Word  Offset of previous "sk"-Record */
    gint32 ofs_prev;
    /* 0x0008   D-Word  Offset of next "sk"-Record     */
    gint32 ofs_next;
    /* 0x000C   D-Word  usage-counter                  */
    gint32 use_count;
    /* 0x0010   D-Word  Size of "sk"-record in bytes   */
    gint32 len;
    /* Security data up to len_sk bytes               */
    guint8 data[];
} RRA_VERBATIM_STRUCT;

/** Security descriptor wrapper object.
 *
 * @ingroup sdc_hive_group
 */
struct SecurityDescriptorCell_
{
    Cell cell;
};

static struct SecurityDescriptorCellData* get_data(SecurityDescriptorCell *kc);
static Hive* get_hive(SecurityDescriptorCell *kc);
static offset get_offset(SecurityDescriptorCell *kc);

static inline struct SecurityDescriptorCellData*
get_data(SecurityDescriptorCell *kc)
{
    return (struct SecurityDescriptorCellData*)cell_get_data(
        security_descriptor_cell_to_cell(kc));
}

static Hive*
get_hive(SecurityDescriptorCell *kc)
{
    return cell_get_hive(security_descriptor_cell_to_cell(kc));
}

static offset
get_offset(SecurityDescriptorCell *kc)
{
    return cell_get_offset(security_descriptor_cell_to_cell(kc));
}

static void
sd_link_into_list(Hive *in_hive, SecurityDescriptorCell *in_cell)
{
    SecurityDescriptorCell *first =
        hive_get_first_security_descriptor_cell(in_hive);

    gint32 sd_ofs = offset_to_begin(get_offset(in_cell));

    if (first == NULL)
    {
        get_data(in_cell)->ofs_prev = sd_ofs;
        get_data(in_cell)->ofs_next = sd_ofs;
    }
    else
    {
        SecurityDescriptorCell *last1 =
            security_descriptor_cell_get_next(first);
        SecurityDescriptorCell *last2 = first;
        while (last1 != NULL)
        {
            last2 = last1;
            last1 = security_descriptor_cell_get_next(last2);
        }

        get_data(in_cell)->ofs_next = get_data(last2)->ofs_next;
        get_data(in_cell)->ofs_prev = get_data(first)->ofs_prev;
        get_data(last2)->ofs_next = sd_ofs;
        get_data(first)->ofs_prev = sd_ofs;
    }
}

static SecurityDescriptorCell *
security_descriptor_cell_alloc_internal (Hive *in_hive, offset in_ofs,
                                         int in_len)
{
    SecurityDescriptorCell *ret_val;
    Cell *new_cell;

    new_cell = cell_alloc (
        in_hive, in_ofs,
        sizeof (struct SecurityDescriptorCellData) + in_len);

    if (new_cell == NULL)
    {
        return NULL;
    }

    ret_val = (SecurityDescriptorCell*)new_cell;

    get_data(ret_val)->cell_head.id = ID_SK_KEY;
    get_data(ret_val)->ofs_prev = 0;
    get_data(ret_val)->ofs_next = 0;
    get_data(ret_val)->use_count = 1;
    get_data(ret_val)->len = in_len;

    sd_link_into_list(in_hive, ret_val);

    return ret_val;
}

static SecurityDescriptorCell*
sdc_find_with_secdesc(Hive *in_hive, const SecurityDescriptor *in_secdesc)
{
    int index = 0;
    SecurityDescriptorCell *sdc =
        hive_get_first_security_descriptor_cell(in_hive);

    while (sdc != NULL)
    {
        const SecurityDescriptor *sd =
            security_descriptor_cell_get_secdesc(sdc);
        rra_debug("Comparing(%d)\n  %s\nand\n  %s\n",
                  index, secdesc_to_sddl(sd), secdesc_to_sddl(in_secdesc));
        if (secdesc_equal(sd, in_secdesc))
        {
            rra_debug("Found equal");
            return sdc;
        }
        index++;
        sdc = security_descriptor_cell_get_next(sdc);
    }

    return NULL;
}

SecurityDescriptorCell*
security_descriptor_cell_alloc (
    Hive *in_hive, offset in_ofs, const SecurityDescriptor *in_secdesc)
{
    SecurityDescriptorCell *ret_val =
        sdc_find_with_secdesc(in_hive, in_secdesc);

    if (ret_val == NULL)
    {
        rra_debug("Creating new cell");
        GByteArray *data = secdesc_to_binary(in_secdesc, 0);
        ret_val = security_descriptor_cell_alloc_internal(in_hive, in_ofs,
                                                          data->len);

        if (ret_val != NULL)
        {
            memcpy(get_data(ret_val)->data, data->data, data->len);
        }

        g_byte_array_free(data, TRUE);
    }
    else
    {
        rra_debug("Using existing cell %p", ret_val);
        security_descriptor_cell_increment_use_count(ret_val);
    }

    return ret_val;
}

gboolean
security_descriptor_cell_unalloc(SecurityDescriptorCell *in_sdc)
{
    rra_info(N_("Unallocating (%#010x)"),
             offset_to_begin(cell_get_offset(
                                 security_descriptor_cell_to_cell(in_sdc))));

    SecurityDescriptorCell *prev = security_descriptor_cell_get_prev(in_sdc);
    SecurityDescriptorCell *next = security_descriptor_cell_get_next(in_sdc);
    get_data(prev)->ofs_next = offset_to_begin(get_offset(next));
    get_data(next)->ofs_prev = offset_to_begin(get_offset(prev));

    return cell_unalloc(security_descriptor_cell_to_cell(in_sdc));
}

Cell*
security_descriptor_cell_to_cell(SecurityDescriptorCell *sdc)
{
    return (Cell*)sdc;
}

SecurityDescriptorCell *
security_descriptor_cell_from_cell (Cell *p)
{
    SecurityDescriptorCell *result = (SecurityDescriptorCell*)p;

    if (get_data(result)->cell_head.id == ID_SK_KEY)
    {
        return result;
    }
    else
    {
        return NULL;
    }
}

int
security_descriptor_cell_get_use_count(SecurityDescriptorCell *in_sdc)
{
    return get_data(in_sdc)->use_count;
}

gboolean
security_descriptor_cell_increment_use_count(
    SecurityDescriptorCell *in_sdc)
{
    get_data(in_sdc)->use_count++;
    return TRUE;
}

gboolean
security_descriptor_cell_decrement_use_count(
    SecurityDescriptorCell *in_sdc)
{
    get_data(in_sdc)->use_count--;
#if 0
    if (security_descriptor_cell_get_use_count(in_sdc) == 0)
    {
        security_descriptor_cell_unalloc(in_sdc);
    }
#endif
    return TRUE;
}

SecurityDescriptorCell*
security_descriptor_cell_get_prev(SecurityDescriptorCell *in_sdc)
{
    guint32 my_offset = offset_to_begin(get_offset(in_sdc));
    guint32 root_sdc_offset =
        offset_to_begin(get_offset(
                            hive_get_first_security_descriptor_cell(
                                get_hive(in_sdc))));

    /* prev/next pointers form a loop so the root cell one is the first */
    if (my_offset == root_sdc_offset)
    {
        return NULL;
    }

    return security_descriptor_cell_from_cell(
        hive_get_cell_from_offset (get_hive(in_sdc),
                                   get_data(in_sdc)->ofs_prev));
}

SecurityDescriptorCell*
security_descriptor_cell_get_next(SecurityDescriptorCell *in_sdc)
{
    guint32 root_sdc_offset =
        offset_to_begin(get_offset(
                            hive_get_first_security_descriptor_cell(
                                get_hive(in_sdc))));

    /* prev/next pointers form a loop so the root cell one is the first */
    if (get_data(in_sdc)->ofs_next == root_sdc_offset)
    {
        return NULL;
    }

    return security_descriptor_cell_from_cell(
        hive_get_cell_from_offset (get_hive(in_sdc),
                                   get_data(in_sdc)->ofs_next));
}

static void
secdesc_cell_data_delete (void *in_data)
{
    if (in_data != NULL)
    {
        secdesc_free((SecurityDescriptor*)in_data);
    }
}

const SecurityDescriptor*
security_descriptor_cell_get_secdesc(SecurityDescriptorCell *in_sdc)
{
    if (in_sdc->cell.data == NULL)
    {
        gsize size = get_data(in_sdc)->len;
        in_sdc->cell.data =
            secdesc_new_parse_binary(get_data(in_sdc)->data, &size);
        in_sdc->cell.deleter = secdesc_cell_data_delete;
    }

    return (const SecurityDescriptor*)in_sdc->cell.data;
}

struct count_data_
{
    int count;
};
typedef struct count_data_ count_data;

static void
increment_count(GTree *counts,
                RRACheckData *in_data,
                SecurityDescriptorCell *in_sdc)
{
    count_data *count = (count_data*)g_tree_lookup(counts, in_sdc);

    if (count == NULL)
    {
        rra_check_warning(in_data,
                          _("SecurityDescriptorCell: %p not linked into list"),
                          offset_to_begin(get_offset(in_sdc)));
        if (in_data->verbosity >= RRA_CHECK_VERBOSITY_WARNING)
        {
            security_descriptor_cell_debug_print(in_sdc);
        }

        count_data *cd = g_new0(count_data, 1);
        cd->count = 1;
        g_tree_insert(counts, in_sdc, cd);
    }
    else
    {
        count->count++;
    }
}

static void
walk_tree_checker(KeyCell *in_kc, RRACheckData *in_data,
                  GTree *in_counts)
{
    if (in_kc == NULL)
    {
        rra_check_warning(in_data, _("NULL key cell"));
        return;
    }

    SecurityDescriptorCell *sdc = key_cell_get_security_descriptor_cell(in_kc);

    increment_count(in_counts, in_data, sdc);

    /* walk into subkeys */
    guint i;
    for (i = 0; i < key_cell_get_number_of_subkeys(in_kc); i++)
    {
        KeyCell *subk = key_cell_get_subkey(in_kc, i);
        walk_tree_checker(subk, in_data, in_counts);
    }
}

static gboolean
foreach_func(gpointer key, gpointer value, gpointer data)
{
    SecurityDescriptorCell *cell = (SecurityDescriptorCell*)key;
    count_data *cd = (count_data*)value;
    RRACheckData *fedata = (RRACheckData*)data;

    if (security_descriptor_cell_get_use_count(cell) != cd->count)
    {
        rra_check_warning(
            fedata,
            _("SecurityDescriptorCell %#010x count off: %d (should be %d)"),
            offset_to_begin(get_offset(cell)),
            security_descriptor_cell_get_use_count(cell),
            cd->count);
    }

    return TRUE;
}

static gboolean
compare_pointers(gconstpointer a, gconstpointer b)
{
    return (char*)a - (char*)b;
}

gboolean
hcheck_security_descriptor_cell_pass(Hive *in_hive, RRACheckData *in_data)
{
    if (in_hive == NULL)
    {
        return FALSE;
    }

    gboolean ret_val = TRUE;

    rra_check_checking(in_data, _("correct use counts"));
    GTree *counts = g_tree_new(compare_pointers);

    SecurityDescriptorCell *sdc =
        hive_get_first_security_descriptor_cell(in_hive);
    while (sdc != NULL)
    {
        g_tree_insert(counts, sdc, g_new0(count_data, 1));
        sdc = security_descriptor_cell_get_next(sdc);
    }

    KeyCell *kc = hive_get_root_key(in_hive);
    walk_tree_checker(kc, in_data, counts);

    g_tree_foreach(counts, foreach_func, in_data);

    g_tree_destroy(counts);

    rra_check_checking(in_data, _("duplicate secdescs"));
    {
        SecurityDescriptorCell *sdc_i =
            hive_get_first_security_descriptor_cell(in_hive);
        while (sdc_i != NULL)
        {
            SecurityDescriptorCell *sdc_j =
                security_descriptor_cell_get_next(sdc_i);
            while (sdc_j != NULL)
            {
                if (secdesc_equal(
                        security_descriptor_cell_get_secdesc(sdc_i),
                        security_descriptor_cell_get_secdesc(sdc_j)))
                {
                    rra_check_warning(
                        in_data,
                        _("SecurityDescriptorCells "
                          "%#010x and %#010x have equal secdescs"),
                        offset_to_begin(get_offset(sdc_i)),
                        offset_to_begin(get_offset(sdc_j)));
                    // I suspect this is common
                    // ret_val = FALSE;
                }

                sdc_j = security_descriptor_cell_get_next(sdc_j);
            }

            sdc_i = security_descriptor_cell_get_next(sdc_i);
        }
    }

    return ret_val;
}

void
security_descriptor_cell_debug_print (SecurityDescriptorCell *in_sdc)
{
    ustring *out = ustr_new();

    if (security_descriptor_cell_get_xml_output(in_sdc, out))
    {
        fprintf(stderr, "%s", ustr_as_utf8(out));
    }

    ustr_free(out);
}


gboolean
security_descriptor_cell_get_xml_output (SecurityDescriptorCell *in_sdc,
                                         ustring *in_output)
{
    ustr_printfa (in_output, "<SecurityDescriptorCell offset=\"%#010x\">\n",
                  offset_to_begin(get_offset(in_sdc)));
    ustr_printfa (in_output, "  <dummy1 value=\"%d\"/>\n",
                  get_data(in_sdc)->dummy1);
    ustr_printfa (in_output, "  <offset_to_prev_cell value=\"%#010x\"/>\n",
                  get_data(in_sdc)->ofs_prev);
    ustr_printfa (in_output, "  <offset_to_next_cell value=\"%#010x\"/>\n",
                  get_data(in_sdc)->ofs_next);
    ustr_printfa (in_output, "  <usage_counter value=\"%d\"/>\n",
                  get_data(in_sdc)->use_count);
    ustr_printfa (in_output, "  <security_data_len value=\"%d\"/>\n",
                  get_data(in_sdc)->len);
    ustr_printfa (in_output, "  <secdesc>\n");
    ustr_printfa (in_output, "%s\n",
                  secdesc_to_sddl(
                      security_descriptor_cell_get_secdesc(in_sdc)));
    ustr_printfa (in_output, "  </secdesc>\n");
    ustr_printfa (in_output, "  <secdesc-binary>\n");
    ustr_hexdumpa (in_output, get_data(in_sdc)->data,
                   0, get_data(in_sdc)->len, TRUE, FALSE);
    ustr_printfa (in_output, "  </secdesc-binary>\n");
    ustr_printfa (in_output, "</SecurityDescriptorCell>\n");
    return TRUE;
}
