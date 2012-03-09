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

#include <stdio.h>
#include <string.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/hive/bin.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/util/check.h>
#include <rregadmin/hive/hcheck_decl.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/hive/defines.h>
#include <rregadmin/hive/hive_offset.h>
#include <rregadmin/hive/log.h>
#include <rregadmin/util/malloc.h>

#define ID_HBIN 0x6E696268

/** The header at the begining of every bin.
 *
 * @internal
 *
 * @ingroup bin_hive_group
 */
struct BinHeader
{
    /** Magic number.
     *
     * Is always "hbin" = 0x6E696268
     */
    gint32 id;
    /** Offset from the 1st bin
     */
    gint32 ofs_from1;
    /** Offset to the next cell (from THIS ONE)
     */
    gint32 ofs_next;
    /** Not sure what this is.
     */
    guint8 unknown1[8];
    /** Last modified time of the bin.
     */
    NTTIME timestamp;
    /** Block-size??? Don't look like it,
     *
     * Since this can be determined using ofs_next this member often
     * contains other random data.
     */
    gint32 len_page;
} RRA_VERBATIM_STRUCT;

/** The organizing structure for a hive.
 *
 * After the hive header comes a series of Bins all the way to the end
 * of the file.  Contained within the bins are Cell_ s.
 *
 * @internal
 *
 * @ingroup bin_hive_group
 */
struct BinData
{
    struct BinHeader head;
    /* 0x0020   First data block starts here           */
    guint8 start_block[];
} RRA_VERBATIM_STRUCT;

/** Wrapper object fore hive bins.
 *
 * @ingroup bin_hive_group
 */
struct Bin_
{
    hive_offset ho;
    Bin *next;
    Bin *prev;
};

static Bin* bin_new (Hive *hdesc, Bin *in_prev, const offset p);
static gboolean bin_header_get_xml_output(const Bin *in_bin,
                                          ustring *in_output);

static gboolean bin_set_offset_next(Bin *previous, Bin *next);
static struct BinData* get_data(Bin *p);
static Hive* get_hive(Bin *b);
static offset get_offset(Bin *b);

#define RRA_DEBUG_BIN FALSE

#if RRA_INCLUDE_DEBUG_CODE && RRA_DEBUG_BIN

#define CHECK_N_RETURN(bin, da_return)              \
    do                                              \
    {                                               \
        if (!bin_is_valid(bin))                     \
        {                                            \
            rra_warning(N_("Bin failed check"));     \
            return (da_return);                      \
        }                                            \
    } while(0)
#else

#define CHECK_N_RETURN(bin, da_return)

#endif

static Hive*
get_hive(Bin *b)
{
    return hive_offset_get_hive((hive_offset*)b);
}

static offset
get_offset(Bin *b)
{
    return hive_offset_get_offset((hive_offset*)b);
}

static inline struct BinData*
get_data(Bin *b)
{
    return (struct BinData*)offset_get_data(get_offset(b));
}

static inline const struct BinData*
get_data_const(const Bin *b)
{
    return (const struct BinData*)offset_get_data(get_offset((Bin*)b));
}

static gboolean
bin_set_offset_next(Bin *previous, Bin *next)
{
    CHECK_N_RETURN(previous, FALSE);
    CHECK_N_RETURN(next, FALSE);

    get_data(previous)->head.ofs_next =
        offset_diff(get_offset(previous), get_offset(next));
    return TRUE;
}

gboolean
bin_init(Hive *hv, offset data, guint32 data_len, Bin *previous_bin)
{
    Bin *me = bin_new(hv, previous_bin, data);

    get_data(me)->head.id = ID_HBIN;
    get_data(me)->head.ofs_from1 = offset_to_begin(data);
    get_data(me)->head.ofs_next = 0;
    if (previous_bin != NULL)
    {
        if (!bin_set_offset_next(previous_bin, me))
        {
            rra_warning(N_("Couldn't set next offset in previous bin"));
            return FALSE;
        }
    }
    get_data(me)->head.unknown1[0] = 0;
    get_data(me)->head.unknown1[1] = 0;
    get_data(me)->head.unknown1[2] = 0;
    get_data(me)->head.unknown1[3] = 0;
    get_data(me)->head.unknown1[4] = 0;
    get_data(me)->head.unknown1[5] = 0;
    get_data(me)->head.unknown1[6] = 0;
    get_data(me)->head.unknown1[7] = 0;

    get_data(me)->head.timestamp = nttime_now();
    get_data(me)->head.len_page = data_len;

    if (!cell_init(hv, me,
                   offset_make_relative(get_offset(me),
                                        sizeof(struct BinHeader)),
                   data_len - sizeof(struct BinHeader)))
    {
        rra_warning(N_("Couldn't initialize cell"));
        return FALSE;
    }

    if (!bin_is_valid(me))
    {
        rra_warning(N_("Bin isn't valid"));
        return FALSE;
    }

    return TRUE;
}

static Bin*
bin_new (Hive *hdesc, Bin *prev, const offset p)
{
    Bin *ret_val = rra_new_type(Bin);

    hive_offset_init(&ret_val->ho, hdesc, p);

    if (prev != NULL)
    {
        prev->next = ret_val;
    }
    ret_val->prev = prev;
    ret_val->next = NULL;

    return ret_val;
}

void
bin_delete (Bin *b)
{
    if (b == NULL)
    {
        return;
    }

    rra_free_type(Bin, b);
}

Bin *
bin_get (Hive *hdesc, Bin *in_prev, offset p)
{
    Bin *result = bin_new(hdesc, in_prev, p);

    if (bin_is_valid (result))
    {
        return result;
    }
    else
    {
        return NULL;
    }
}

offset
bin_get_offset(Bin *b)
{
    return get_offset(b);
}

gboolean
bin_is_valid (Bin *p)
{
    if (p == NULL)
    {
        rra_message(N_("Checking a NULL bin"));
        return FALSE;
    }

    if (get_data(p)->head.id != ID_HBIN)
    {
        rra_message(N_("Invalid id"));
        return FALSE;
    }

    return TRUE;
}

gint32
bin_get_len_page(Bin *b)
{
    CHECK_N_RETURN(b, 0);
    return get_data(b)->head.len_page;
}

gint32
bin_get_ofs_next(Bin *b)
{
    CHECK_N_RETURN(b, 0);
    return get_data(b)->head.ofs_next;
}

guint32
bin_size(Bin *b)
{
    struct BinData *bd = get_data(b);
    /*
     * I haven't figured out the exact nature of the relationship between
     * ofs_next and len_page so this is funky, but seems to work.
     */
    CHECK_N_RETURN(b, 0);
    return ((bd->head.ofs_next > 0)
            ? bd->head.ofs_next
            : bd->head.len_page);
}

guint32
bin_pages(Bin *b)
{
    CHECK_N_RETURN(b, 0);
    return bin_size(b) / HIVE_PAGE_LEN;
}

guint32
bin_offset_to_begin(Bin *b)
{
    CHECK_N_RETURN(b, 0);
    return offset_to_begin(get_offset(b));
}

gboolean
bin_scan (Bin *p, struct used_stats *stats)
{
    CHECK_N_RETURN(p, FALSE);
    bzero (stats, sizeof (struct used_stats));

    Cell *b = NULL;

    stats->data += sizeof(struct BinHeader);

    for (b = bin_first_cell (p);
         b != NULL;
         b = cell_get_next (b))
    {
        if (cell_is_allocd (b))
        {
            stats->used += cell_size (b);
        }
        else
        {
            stats->free += cell_size (b);
        }
    }

    stats->lost = bin_size(p)
        -  (stats->used + stats->free + stats->data);

    return TRUE;
}

gboolean
bin_set_dirty (Bin *in_bin)
{
    CHECK_N_RETURN(in_bin, FALSE);

    hive_set_dirty(get_hive(in_bin));

    /* reset last modified time */
    get_data(in_bin)->head.timestamp = nttime_now();

    return TRUE;
}

Cell *
bin_first_cell (Bin *p)
{
    CHECK_N_RETURN(p, NULL);
    offset cell_ofs = offset_make_relative(get_offset(p),
                                           sizeof(struct BinHeader));
    return cell_get(get_hive(p), p, cell_ofs);
}

Cell *
bin_find_free_cell (Bin *p, guint32 size)
{
    CHECK_N_RETURN(p, NULL);
    Cell *c = NULL;

    for (c = bin_first_cell (p);
         c != NULL;
         c = cell_get_next (c))
    {
        if (!cell_is_allocd (c)
            && (cell_size (c) >= size))
        {
            return c;
        }
    }

    return NULL;
}

Bin *
bin_get_next_bin (Bin *p)
{
    CHECK_N_RETURN(p, NULL);
    offset next;

    if (p->next == NULL)
    {
        if (get_data(p)->head.ofs_next != 0)
        {

            next = offset_make_relative(get_offset(p),
                                        get_data(p)->head.ofs_next);

            if (offset_is_valid(next))
            {
                if (hive_is_valid_offset(get_hive(p), offset_to_begin(next)))
                {
                    Bin *ret_val = bin_get (get_hive(p), p, next);
                    p->next = ret_val;
                }
            }
        }
    }

    return p->next;
}

gboolean
bin_within(Bin *p, offset check_ptr)
{
    CHECK_N_RETURN(p, FALSE);
    guint32 begin_ofs = offset_to_begin(get_offset(p));
    guint32 end_ofs = begin_ofs + bin_size(p);
    int ret_val = (offset_to_begin(check_ptr) >= begin_ofs
                   && offset_to_begin(check_ptr) < end_ofs);
    return ret_val ? TRUE : FALSE;
}

static gboolean
bin_header_get_xml_output(const Bin *in_bin, ustring *in_output)
{
    ustr_printfa (in_output, "  <head>\n");
    ustr_printfa (in_output,
                  "    <id expected=\"0x6e696268\", value=\"%#010x\">\n",
                  get_data_const(in_bin)->head.id);
    ustr_printfa (in_output, "    <ofs_from1 value=\"%#010x\"/>\n",
                  get_data_const(in_bin)->head.ofs_from1);
    ustr_printfa (in_output, "    <ofs_next value=\"%#010x\"/>\n",
                  get_data_const(in_bin)->head.ofs_next);
    const char *hex_repr = "";
    // tohex (get_data_const(in_bin)->head.unknown1, 0,
    // sizeof(get_data_const(in_bin)->head.unknown1));
    ustr_printfa (in_output, "    <unknown1 value=\"%s\"/>\n", hex_repr);
    // free(hex_repr);
    ustr_printfa (in_output,
                  "    <timestamp value=\"%#" G_GINT64_MODIFIER "x\"/>\n",
                  get_data_const(in_bin)->head.timestamp);
    ustr_printfa (in_output, "    <len_page value=\"%#010x\"/>\n",
                  get_data_const(in_bin)->head.len_page);
    ustr_printfa (in_output, "    <offset_to_end value=\"%#010x\"/>\n",
                  offset_to_end(get_offset((Bin*)in_bin)));
    ustr_printfa (in_output, "    <size value=\"%#010x\"/>\n",
                  bin_size((Bin*)in_bin));

    ustr_printfa (in_output, "  </head>\n");
    return TRUE;
}

void
bin_debug_print (Bin *in_bin, gboolean in_verbose)
{
    ustring *xml_out = ustr_new();
    bin_get_xml_output(in_bin, xml_out, in_verbose);
    fprintf (stderr, ustr_as_utf8(xml_out));
    ustr_free(xml_out);
}

gboolean
bin_get_xml_output (const Bin *in_bin, ustring *in_output,
                    gboolean in_verbose)
{
    ustr_printfa (in_output, "<Bin offset=\"%#010x\">\n",
                  offset_to_begin(get_offset((Bin*)in_bin)));
    bin_header_get_xml_output(in_bin, in_output);
    if (in_verbose)
    {
        Cell *c;
        ustr_printfa (in_output, "  <cells>\n");
        for (c = cell_get(get_hive((Bin*)in_bin), (Bin*)in_bin,
                          offset_make_relative(get_offset((Bin*)in_bin),
                                               sizeof(struct BinHeader)));
             c != NULL;
             c = cell_get_next (c))
        {
            ustr_printfa (in_output, "    ");
            cell_get_xml_output (c, in_output, FALSE);
        }
        ustr_printfa (in_output, "  </cells>\n");
    }

    ustr_printfa (in_output, "</Bin>\n");
    return TRUE;
}

gboolean
hcheck_bin_pass(Hive *in_hive, RRACheckData *in_data)
{
    if (in_hive == NULL)
    {
        return FALSE;
    }

    gboolean ret_val = TRUE;

    guint size = 0;

    Bin *last_b = NULL;
    Bin *b;
    for (b = hive_get_first_bin(in_hive);
         b != NULL;
         last_b = b, b = bin_get_next_bin(b))
    {
        if (!bin_is_valid(b))
        {
            ret_val = FALSE;
            break;
        }

        rra_check_checking(in_data, _("for a valid size"));
        if ((bin_size(b) % HIVE_PAGE_LEN) != 0)
        {
            rra_check_error(in_data,
                            _("bin size not a multiple of %d"),
                            HIVE_PAGE_LEN);
            ret_val = FALSE;
        }

        rra_check_checking(in_data, _("for valid end offset"));
        offset end_of_bin = offset_make_relative(bin_get_offset(b),
                                                 bin_size(b) - 1);
        if (offset_to_begin(end_of_bin) == 0)
        {
            rra_check_error(in_data,
                            _("generated end offset bad"));
            ret_val = FALSE;
        }

        rra_check_checking(in_data, _("for valid offsets"));
        if (get_data(b)->head.ofs_from1
            != (gint32)offset_to_begin(get_offset(b)))
        {
            rra_check_error(in_data, _("offsets don't match"));
            ret_val = FALSE;
        }

        size += bin_size(b);
    }

    rra_check_checking(in_data, _("total bin size"));
    if (size != hive_get_data_size(in_hive))
    {
        rra_check_error(in_data,
                        _("hive data size and total bin size don't match"));
        ret_val = FALSE;
    }

    rra_check_checking(in_data, _("last bin same"));
    if (last_b != hive_get_last_bin(in_hive))
    {
        rra_check_error(in_data, _("last bins not the same"));
        ret_val = FALSE;
    }

    offset end_ofs = hive_get_trailer_offset(in_hive);

    while (offset_is_valid(end_ofs))
    {
        rra_check_checking(in_data, _("no bins in trailer"));
        Bin *tmpb = bin_get(in_hive, NULL, end_ofs);
        if (tmpb != NULL)
        {
            rra_check_warning(in_data,
                              _("found bin tag in trailer"));
            /* ret_val = FALSE; */
        }

        end_ofs = offset_make_relative(end_ofs, HIVE_PAGE_LEN);
    }

    return ret_val;
}
