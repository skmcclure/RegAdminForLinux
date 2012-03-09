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

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/hive/hive.h>
#include <rregadmin/util/check.h>
#include <rregadmin/util/binary_search.h>
#include <rregadmin/hive/hcheck_decl.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/hive/bin.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/security_descriptor_cell.h>
#include <rregadmin/hive/log.h>
#include <rregadmin/util/path.h>
#include <rregadmin/util/offsets.h>
#include <rregadmin/hive/init.h>
#include <rregadmin/util/fs.h>

#define REGF_HEADER_SIZE HIVE_PAGE_LEN
#define HIVE_ALLOC_SIZE 0x40000

/** Header for the hive.
 *
 * The first page of the hive file is some kind of header, lot of
 * it's contents is unknown, and seems to be mostly NULLs anyway.
 *
 * \note that this is the only place in the registry I've been
 * able to find _any_ kind of checksumming
 *
 * @internal
 *
 * @ingroup hive_hive_group
 */
struct HiveHeader
{
    /** Magic number
     *
     * Is "regf" = 0x66676572
     */
    guint32 id;
    /** Not sure what this is.
     */
    guint32 update_counter1;
    /** Not sure what this is
     *
     * Always the same value as at update_counter1
     */
    guint32 update_counter2;
    /** last modify date in WinNT date-format
     */
    NTTIME timestamp;
    /** Version structure.
     */
    HiveVersion version;
    /** Offset to the root key
     */
    guint32 ofs_rootkey;
    /** Offset to the last bin.
     */
    guint32 lastblock;
    /** Not sure what this is.
     *
     * Seems to be 1 most of the time.
     */
    guint32 unknown7;
    /** The name of the hive in UTF-16LE format.
     *
     * I'm not sure this is correct, but for versions 1,5,0 this puts
     * the first non zero data in checksum.  ALso it makes the header
     * 512 bytes in size which sounds like it'd be correct.  For
     * version types 1,3,0 it looks like this bit is much smaller and
     * there's some sort of data after it and before the checksum.  In
     * all cases the first bits of this are the name of the hive in
     * UTF16.
     */
    guint16 name[32];
    /** Padding
     *
     * The samba source seems to think there's padding here and as far
     * as I can tell this data isn't important so I'll go along with
     * samba and call it padding
     */
    guint16 padding1[198];
    /** XOR of all words in the header (starting with 0)
     */
    guint32 checksum;

    /** The rest of the page that seems unused.
     *
     * I'm not going to define it here because it doesn't seem to be useful
     * to do so.
     */
    // guint8 padding2[4096 - 512];
} RRA_VERBATIM_STRUCT;

/** Object containing the data for handling a hive.
 *
 * @internal
 *
 * @ingroup hive_hive_group
 */
struct Hive_
{
    /** Often the hive's filename
     */
    char *name;

    /** Current state of hive
     */
    int state;

    /** Number of bins, total
     */
    guint32  bins;

    /** Number of pages.
     *
     * This should always be >= to bins
     */
    guint32 pages;

    /** total # of bytes in useblk
     */
    guint32  used_bytes;

    /** total # of bytes in unuseblk
     */
    guint32 unused_bytes;

    /** Contains the actual data of the hive.
     */
    offset_holder *oh;

    /** Place to cache the root key.
     */
    KeyCell *root_key;

    /** Place to cache the first SecurityDescriptorCell
     */
    SecurityDescriptorCell *sd_cell;

    /** Cache of bins.
     */
    GPtrArray *bin_cache;

    /** Cache of all created Cell_ s.
     */
    GTree *cell_cache;
};

static gboolean DO_HIVE_PRELOAD_ON_OPEN = FALSE;

static void hive_struct_free (Hive *hdesc);
static Hive * hive_struct_alloc (const char *name);
static guint32 generate_checksum(const struct HiveHeader *header);
static void reset_checksum(Hive *in_hive);
static struct HiveHeader* get_head(Hive *hv);
static void hive_preload_bins(Hive *in_hive);

gboolean
hive_free (Hive *in_hive)
{
    rra_debug("Freeing hive %s(%p)", hive_get_filename(in_hive), in_hive);

    hive_struct_free (in_hive);

    return TRUE;
}

static void
hive_struct_free (Hive *hdesc)
{
    if (hdesc->cell_cache != NULL)
    {
        g_tree_destroy(hdesc->cell_cache);
        hdesc->cell_cache = NULL;
    }

    if (hdesc->bin_cache != NULL)
    {
        while (hdesc->bin_cache->len > 0)
        {
            Bin *b = (Bin*)g_ptr_array_remove_index_fast(hdesc->bin_cache, 0);
            bin_delete(b);
        }
        g_ptr_array_free(hdesc->bin_cache, TRUE);
        hdesc->bin_cache = NULL;
    }

    if (hdesc->oh != NULL)
    {
        offset_holder_free(hdesc->oh);
        hdesc->oh = NULL;
    }

    if (hdesc->name != NULL)
    {
        g_free (hdesc->name);
        hdesc->name = NULL;
    }

    g_free (hdesc);
}

gboolean
hive_write (Hive *hdesc, const char *filename)
{
    int len;

    int filedesc;

    filedesc = open (filename, O_WRONLY | O_CREAT | O_TRUNC,
                     S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (filedesc == -1)
    {
        rra_warning(N_("open(%s) failed, FILE NOT WRITTEN: %s"),
                    filename, strerror(errno));
        return FALSE;
    }

    len = write (filedesc, offset_holder_get_buf(hdesc->oh),
                 offset_holder_buf_size(hdesc->oh));
    close(filedesc);
    if (len != (int)offset_holder_buf_size(hdesc->oh))
    {
        rra_warning(N_("write of %s failed: %s"), filename, strerror(errno));
        return FALSE;
    }
    else
    {
        hdesc->state &= (~HMODE_DIRTY);
        return TRUE;
    }
}

static gint
local_offset_compare(gconstpointer a, gconstpointer b,
                     gpointer DECLARE_UNUSED(data))
{
    return GPOINTER_TO_INT(a) - GPOINTER_TO_INT(b);
}

static Hive *
hive_struct_alloc (const char *name)
{
    Hive *hdesc;
    hdesc = g_new0(Hive, 1);

    hdesc->name = g_strdup (name);
    hdesc->cell_cache = g_tree_new_full(local_offset_compare, 0, 0,
                                        (GDestroyNotify)cell_delete);
    hdesc->bin_cache = g_ptr_array_new();

    return hdesc;
}

gboolean
hive_is_hive (const char *filename)
{
    struct stat fo_stat;

    if (stat(filename, &fo_stat) == 0)
    {
        if (fo_stat.st_size == 0)
        {
            return FALSE;
        }
        else
        {
            Hive *hv = hive_read(filename);
            if (hv == NULL)
            {
                return FALSE;
            }
            hive_free(hv);
        }
        return TRUE;
    }

    return FALSE;
}

Hive *
hive_read (const char *filename)
{
    int filedesc;
    guint8 *buf = NULL;
    int fmode, r;       //,vofs;
    struct stat sbuf;
    Hive *ret_val = NULL;

    ustring *fn = ustr_create(filename);
    if (!find_file_casei_bang(fn))
    {
        rra_message(N_("Couldn't find hive file: %s"), filename);
        return NULL;
    }

    fmode = O_RDONLY;

    filedesc = open (ustr_as_utf8(fn), fmode);

    if (filedesc < 0)
    {
        rra_warning(N_("Couldn't open hive file %s"), ustr_as_utf8(fn));
        return NULL;
    }

    if (fstat (filedesc, &sbuf) == -1)
    {
        close(filedesc);
        rra_warning(N_("Couldn't fstat hive file %s"), ustr_as_utf8(fn));
        return NULL;
    }

    /* Read the whole file */
    buf = g_new(guint8, sbuf.st_size);
    r = read (filedesc, buf, sbuf.st_size);
    if (r < sbuf.st_size)
    {
        rra_warning(N_("Didn't read as many bytes (%d) as I wanted to: %d"),
                    r, sbuf.st_size);
        g_free(buf);
    }
    else
    {
        ret_val = hive_create(ustr_as_utf8(fn), buf, sbuf.st_size);
        if (ret_val == NULL)
        {
            rra_message(N_("failed opening %s"), ustr_as_utf8(fn));
        }
    }

    close(filedesc);

    rra_info(N_("Loaded hive: %s"), filename);

    return ret_val;
}

static struct HiveHeader*
get_head(Hive *hv)
{
    return (struct HiveHeader*)offset_holder_get_header(hv->oh);
}

Hive *
hive_create(const char *name, guint8 *buffer, int len)
{
    Hive *ret_val = NULL;
    Cell *root_cell = NULL;

    /* one of the few entry points to the library so call rra_hive_init */
    rra_hive_init();

    if (buffer == NULL || len == 0)
    {
        return NULL;
    }

    if (NULL == (ret_val = hive_struct_alloc (name)))
    {
        rra_message(N_("couldn't allocate hive structure"));
        return NULL;
    }

    ret_val->state = 0;
    /* Pull some useful info */
    ret_val->oh = offset_holder_new_with_header(buffer,
                                                REGF_HEADER_SIZE,
                                                len);
    hive_preload_bins(ret_val);

    root_cell = hive_get_cell_from_offset (
        ret_val, get_head(ret_val)->ofs_rootkey);

    ret_val->root_key = key_cell_from_cell (root_cell);

    if (!hive_is_valid(ret_val))
    {
        rra_message(N_("hive is invalid"));
        hive_free(ret_val);
        return NULL;
    }

    if (DO_HIVE_PRELOAD_ON_OPEN)
    {
        hive_preload(ret_val);
    }

    return ret_val;
}

static void
hive_preload_bins(Hive *in_hive)
{
    offset ofs = offset_holder_make_offset(in_hive->oh, 0);
    Bin *first_bin = bin_get (in_hive, NULL, ofs);
    Bin *cur_bin;
    for (cur_bin = first_bin;
         cur_bin != 0;
         cur_bin = bin_get_next_bin(cur_bin))
    {
        g_ptr_array_add(in_hive->bin_cache, cur_bin);
    }
}

void
hive_preload(Hive *in_hive)
{
    int i;
    for (i = 0; i < in_hive->bin_cache->len; i++)
    {
        Bin *b = (Bin*)g_ptr_array_index(in_hive->bin_cache, i);
        Cell *c;
        for (c = bin_first_cell(b);
             c != NULL;
             c = cell_get_next(c))
        {
        }
    }
}

guint32
hive_get_trailer_size(const Hive *in_hive)
{
    return offset_holder_size(in_hive->oh)
        - offset_to_begin(bin_get_offset(hive_get_last_bin(in_hive)))
        - bin_size(hive_get_last_bin(in_hive));
}

offset
hive_get_trailer_offset(const Hive *in_hive)
{
    return offset_holder_make_offset(
        in_hive->oh, get_head((Hive*)in_hive)->lastblock);
}

static void
reset_checksum(Hive *in_hive)
{
    get_head(in_hive)->checksum = generate_checksum(get_head(in_hive));
}

static void
set_last_bin(Hive *in_hive, Bin *in_last_bin)
{
    get_head(in_hive)->lastblock =
        offset_to_begin(bin_get_offset(in_last_bin))
        + bin_size(in_last_bin);
    reset_checksum(in_hive);
}

Hive *
hive_new (const ustring *name)
{
    Hive *ret_val = NULL;

    int first_bin_size = HIVE_PAGE_LEN;
    int default_size = HIVE_ALLOC_SIZE;

    guint8 *buf = g_new0(guint8, default_size);

    if (NULL == (ret_val = hive_struct_alloc (ustr_as_utf8(name))))
        return NULL;

    ret_val->oh = offset_holder_new_with_header(buf, REGF_HEADER_SIZE,
                                                default_size);
    ret_val->state = 0;

    // Initialize the header
    struct HiveHeader *head = get_head(ret_val);
    head->id = HIVE_MAGIC;
    head->update_counter1 = 1;
    head->update_counter2 = head->update_counter1;
    head->timestamp = nttime_now();
    head->version.major = 1;
    head->version.minor = 5;
    head->version.release = 0;
    head->version.build = 1;
    head->ofs_rootkey = 0x20;
    // don't know what it means, but all the hives I've looked at have
    // it set to this.
    head->unknown7 = 0x1;
    GByteArray *name_arr = ustr_encode(name, USTR_TYPE_UTF16LE);
    memcpy((guint8*)head->name, name_arr->data,
           MIN(name_arr->len, sizeof(head->name)));
    g_byte_array_free(name_arr, TRUE);

    // Create first bin
    offset start_ofs = offset_holder_make_offset(ret_val->oh, 0);
    if (!bin_init(ret_val, start_ofs, first_bin_size, 0))
    {
        hive_free(ret_val);
        return NULL;
    }

    hive_preload_bins(ret_val);

    set_last_bin(ret_val, hive_get_first_bin(ret_val));

    ret_val->root_key = key_cell_alloc_root(ret_val);
    if (ret_val->root_key == NULL)
    {
        hive_free(ret_val);
        return NULL;
    }

    head->ofs_rootkey =
        offset_to_begin(cell_get_offset(key_cell_to_cell(ret_val->root_key)));
    reset_checksum(ret_val);

    if (!hive_is_valid(ret_val))
    {
        hive_free(ret_val);
        return NULL;
    }

    return ret_val;
}


const HiveVersion*
hive_get_version(const Hive *hdesc)
{
    return &get_head((Hive*)hdesc)->version;
}

gboolean
hive_is_valid(const Hive *hdesc)
{
    struct HiveHeader *head = get_head((Hive*)hdesc);

    // id
    if (head->id != HIVE_MAGIC)
    {
        rra_warning(N_("incorrect magic number: %#010x (should be %#010x)"),
                    head->id, HIVE_MAGIC);
        return FALSE;
    }

    // version
    if (head->version.major != 1)
    {
        rra_warning(N_("unknown major version number: %d"),
                    head->version.major);
        return FALSE;
    }

    if (head->version.minor != 3
        && head->version.minor != 5)
    {
        rra_warning(N_("unknown major/minor combo: %d/%d"),
                    head->version.major, head->version.minor);
        return FALSE;
    }

    return TRUE;
}

int
hive_get_state(const Hive *hv)
{
    return hv->state;
}

const char *
hive_get_filename(const Hive *hv)
{
    return hv->name;
}

void
hive_set_dirty(Hive *hv)
{
    hv->state |= HMODE_DIRTY;
    get_head(hv)->timestamp = nttime_now();
    reset_checksum(hv);
}

gboolean
hive_is_dirty(Hive *hv)
{
    return hv->state & HMODE_DIRTY;
}

guint32
hive_size(const Hive *hv)
{
    return offset_holder_buf_size(hv->oh);
}

guint32
hive_get_data_size(const Hive *in_hive)
{
    return get_head((Hive*)in_hive)->lastblock;
}

guint32
hive_get_page_count(const Hive *hv)
{
    if (hv->pages == 0)
    {
        struct used_stats stats;
        hive_collect_stats((Hive*)hv, &stats);
    }
    return hv->pages;
}

guint32
hive_get_bin_count(const Hive *hv)
{
    if (hv->bins == 0)
    {
        struct used_stats stats;
        hive_collect_stats((Hive*)hv, &stats);
    }
    return hv->bins;
}

Cell*
hive_get_cell_from_offset(Hive *hv, guint in_ofs)
{
    return hive_get_cell (hv, hive_get_offset(hv, in_ofs));
}

Cell *
hive_get_cell (Hive *hdesc, offset in_ofs)
{
    if (offset_is_valid(in_ofs))
    {
        return cell_get(hdesc, NULL, in_ofs);
    }
    else
    {
        return NULL;
    }
}

offset
hive_get_offset(Hive *hdesc, guint32 in_ofs)
{
    if (hive_is_valid_offset(hdesc, in_ofs))
    {
        return offset_holder_make_offset(hdesc->oh, in_ofs);
    }
    else
    {
        offset o = INVALID_OFFSET;
        return o;
    }
}

gboolean
hive_is_valid_offset(const Hive *hdesc, guint32 offset)
{
    return offset < get_head((Hive*)hdesc)->lastblock;
}

void
hive_collect_stats (Hive *hdesc,
                    struct used_stats *stats)
{
    Bin *last_p = NULL;
    int i;

    // One for the header
    hdesc->pages = 1;
    hdesc->bins = 0;

    stats->used = 0;
    stats->free = 0;
    stats->data = 0;
    stats->lost = 0;

    stats->data += REGF_HEADER_SIZE;

    for (i = 0; i < hdesc->bin_cache->len; i++)
    {
        Bin *p = (Bin*)g_ptr_array_index(hdesc->bin_cache, i);
        struct used_stats bin_stats;

        hdesc->bins++;
        hdesc->pages += bin_pages(p);

        bin_scan(p, &bin_stats);

        stats->used += bin_stats.used;
        stats->free += bin_stats.free;
        stats->data += bin_stats.data;
        stats->lost += bin_stats.lost;

    }

    // Version 1,5 files can have blocks at the end of the file unused
    // by bins.  I wonder if this data has any significance?
    last_p = hive_get_last_bin(hdesc);
    stats->lost +=
        (hive_size(hdesc)
         - (offset_to_begin(bin_get_offset(last_p))
            + bin_size(last_p)
            + REGF_HEADER_SIZE));
}

#if 0
static gint compare_bin_to_offset(gconstpointer in_bin,
                                  gconstpointer in_offset);

static gint compare_bin_to_offset(gconstpointer in_bin,
                                  gconstpointer in_offset)
{
    Bin *bin = (Bin*)in_bin;
    offset* ofs = (offset*)in_offset;

    offset_debug_print(*ofs);

    gint ofs_cmp_val = offset_compare(bin_get_offset(bin), *ofs);

    printf("Comparing Bin(%d,%d) to offset(%d) => %d\n",
           offset_to_begin(bin_get_offset(bin)),
           bin_size(bin),
           offset_to_begin(*ofs),
           ofs_cmp_val);

    if (ofs_cmp_val < 0)
    {
        return -1;
    }

    if (ofs_cmp_val > 0 && bin_within(bin, *ofs))
    {
        return 0;
    }

    return 1;
}

Bin *
hive_find_bin (Hive *in_hive, offset p)
{
    gint bin_loc = rra_binary_search_gptrarray(in_hive->bin_cache,
                                               &p,
                                               compare_bin_to_offset,
                                               NULL);

    if (bin_loc == -1)
    {
        return NULL;
    }

    return (Bin*)g_ptr_array_index(in_hive->bin_cache, bin_loc);
}

#else

Bin *
hive_find_bin (Hive *hdesc, offset p)
{
Bin *h  = hive_get_first_bin(hdesc);

    if (!offset_is_valid(p))
    {
        return h;
    }

    do
    {
        if (bin_within(h, p))
        {
            return h;
        }
    }
    while ((h = bin_get_next_bin (h)));

    return NULL;
}

#endif

Bin *
hive_get_last_bin (const Hive *hdesc)
{
    return (Bin*)g_ptr_array_index(hdesc->bin_cache,
                                   hdesc->bin_cache->len - 1);
}

Bin *
hive_get_first_bin (Hive *hdesc)
{
    return (Bin*)g_ptr_array_index(hdesc->bin_cache, 0);
}

Cell *
hive_find_free_cell (Hive *hdesc, int size)
{
    int i;

    for (i = 0; i < hdesc->bin_cache->len; i++)
    {
        Bin *p = (Bin*)g_ptr_array_index(hdesc->bin_cache, i);

        rra_info(N_("Looking for free cell in Bin %#010x"),
                 offset_to_begin(bin_get_offset(p)));
        Cell *r = NULL;
        r = bin_find_free_cell (p, size);
        if (r != NULL)
        {
            return r;
        }
    }

    {
        rra_info(N_("No free cells large enough existing."));

        // 100 is at least the size of the bin header
        guint32 needed_size = size + 100;
        // Even it to block size
        needed_size += (HIVE_PAGE_LEN - (needed_size % HIVE_PAGE_LEN));

        if (needed_size > hive_get_trailer_size(hdesc))
        {
            guint32 enlarge_size =
                needed_size
                + ((needed_size + offset_holder_buf_size(hdesc->oh))
                   % HIVE_ALLOC_SIZE);

            rra_debug("Adding %d bytes to hive", enlarge_size);

            offset new_trailer =
                offset_holder_grow(hdesc->oh, enlarge_size);

            if (!offset_is_valid(new_trailer))
            {
                rra_message("Failed to enlarge hive");
                return NULL;
            }
        }

        offset new_bin_offset = hive_get_trailer_offset(hdesc);

        if (!bin_init(hdesc, new_bin_offset, needed_size,
                      hive_get_last_bin(hdesc)))
        {
            rra_message(N_("Failed to init bin at %#010x"),
                        offset_to_begin(new_bin_offset));
            return NULL;
        }

        Bin *new_bin = bin_get_next_bin(hive_get_last_bin(hdesc));
        g_ptr_array_add(hdesc->bin_cache, new_bin);
        set_last_bin(hdesc, new_bin);

        Cell *ret_val = bin_first_cell(hive_get_last_bin(hdesc));
        if (ret_val == NULL)
        {
            rra_message(N_("Couldn't get first cell from new bin"));
            return NULL;
        }
        return ret_val;
    }
}

KeyCell *
hive_get_root_key (Hive *hdesc)
{
    return hdesc->root_key;
}

SecurityDescriptorCell*
hive_get_first_security_descriptor_cell(Hive *in_hive)
{
    if (in_hive->sd_cell == NULL)
    {
        /*
         * The security descriptors form a loop with next/prev offsets
         * so the security descriptor cell for the root cell will be
         * considered the first one.
         *
         * @warning don't use security_descriptor_cell_get_next/prev in
         *          here else you'll create a loop.
         */
        KeyCell *kc = hive_get_root_key(in_hive);
        if (kc != NULL)
        {
            in_hive->sd_cell = key_cell_get_security_descriptor_cell(kc);
        }
    }

    return in_hive->sd_cell;
}

ustring *
hive_get_name (const Hive *hdesc)
{
    ustring *ret_val;

    ret_val = ustr_new();

    ustr_strnset_type(ret_val, USTR_TYPE_UTF16LE,
                      (char*)get_head((Hive*)hdesc)->name,
                      sizeof (get_head((Hive*)hdesc)->name));

    return ret_val;
}

static guint32
generate_checksum(const struct HiveHeader *header)
{
    typedef guint32 sum_type;
    guint32 i;

    sum_type sum1 = 0;
    sum_type *dw = (sum_type*)header;

    for (i = 0;
         i < ((sizeof(struct HiveHeader) / sizeof(sum_type)) - 1);
         i++)
    {
        sum1 ^= dw[i];
    }

    return sum1;
}

gboolean
hive_register_cell(Hive *hv, Cell *c)
{
    if (c == NULL)
    {
        rra_message(N_("Passed null Cell"));
        return FALSE;
    }

    gpointer possible_cell =
        g_tree_lookup(hv->cell_cache,
                      GUINT_TO_POINTER(offset_to_begin(cell_get_offset(c))));

    if (possible_cell != c)
    {
        g_tree_insert(hv->cell_cache,
                      GUINT_TO_POINTER(offset_to_begin(cell_get_offset(c))),
                      c);
    }
    else
    {
        rra_message(N_("Attempt to register the same cell twice."));
    }

    return TRUE;
}

gboolean
hive_unregister_cell(Hive *hv, Cell *c)
{
    if (c == NULL)
    {
        rra_message(N_("Passed null Cell"));
        return FALSE;
    }

    g_tree_remove(hv->cell_cache,
                  GUINT_TO_POINTER(offset_to_begin(cell_get_offset(c))));
    return TRUE;
}

gpointer
hive_retrieve_cell(Hive *hv, offset ofs)
{
    gpointer ret_val = g_tree_lookup(hv->cell_cache,
                                     GUINT_TO_POINTER(offset_to_begin(ofs)));
    return (Cell*)ret_val;
}

offset_list*
hive_get_cell_offset_list(Hive *in_hive)
{
    offset_list *ret_val = ofslist_new(in_hive->oh);
    int i;

    for (i = 0; i < in_hive->bin_cache->len; i++)
    {
        Bin *b = (Bin*)g_ptr_array_index(in_hive->bin_cache, i);
        Cell *c;
        for (c = bin_first_cell(b);
             c != NULL;
             c = cell_get_next(c))
        {
            offset cofs = cell_get_offset(c);
            ofslist_add(ret_val, cofs);
        }
    }

    return ret_val;
}

void
hive_debug_print (const Hive *in_hive, gboolean in_verbose)
{
    ustring *xml_out = ustr_new();
    hive_get_xml_output(in_hive, xml_out, in_verbose);
    fprintf (stderr, ustr_as_utf8(xml_out));
    ustr_free(xml_out);
}

gboolean
hive_get_xml_output (const Hive *in_hive, ustring *in_output,
                     gboolean in_verbose)
{
    struct HiveHeader *head = get_head((Hive*)in_hive);

    ustr_printfa (in_output, "<Hive>\n");

    ustr_printfa (in_output, "  <filename value=\"%s\"/>\n",
                  hive_get_filename(in_hive));

    ustr_printfa (in_output, "  <head>\n");
    ustr_printfa (in_output,
                  "    <id expected=\"0x66676572\", value=\"%#010x\"/>\n",
                  head->id);
    ustr_printfa (in_output, "    <update_counter1 value=\"%u\"/>\n",
                  head->update_counter1);
    ustr_printfa (in_output, "    <update_counter2 value=\"%u\"/>\n",
                  head->update_counter2);
    ustring *timestamp = nttime_to_str(head->timestamp);
    if (timestamp != NULL)
    {
        ustr_printfa (in_output, "    <timestamp value=\"%s\"/>\n",
                      ustr_as_utf8(timestamp));
        ustr_free(timestamp);
    }
    else
    {
        ustr_printfa (in_output,
                      "    <timestamp value=\"%" G_GINT64_FORMAT "\"/>\n",
                      head->timestamp);
    }

    ustr_printfa (in_output,
                  "    <major_version expected=\"1\", value=\"%u\"/>\n",
                  head->version.major);
    ustr_printfa (in_output,
                  "    <minor_version expected=\"3,5\", value=\"%u\"/>\n",
                  head->version.minor);
    ustr_printfa (in_output,
                  "    <release_version expected=\"0\", value=\"%u\"/>\n",
                  head->version.release);
    ustr_printfa (in_output,
                  "    <build_version expected=\"1\", value=\"%u\"/>\n",
                  head->version.build);
    ustr_printfa (in_output,
                  "    <ofs_rootkey value=\"%#010x\"/>\n", head->ofs_rootkey);
    ustr_printfa (in_output,
                  "    <lastblock value=\"%#010x\"/>\n", head->lastblock);
    ustr_printfa (in_output,
                  "    <unknown7 expected=\"0x1\", value=\"%#010x\"/>\n",
                  head->unknown7);
    // ustr_printfa (in_output,
    //               "name:                   %ls\n", (wchar_t*)head->name);
    ustr_printfa (in_output, "    <name>\n");
    ustr_printfa (in_output, "    </name>\n");
    ustr_printfa (in_output, "    <padding1>\n");
    ustr_printfa (in_output, "    </padding1>\n");
    ustr_printfa (in_output,
                  "    <checksum value=\"%#010x\"/>\n", head->checksum);
    guint8 *padding2 = offset_holder_get_header(in_hive->oh);
    padding2 += sizeof(struct HiveHeader);
    ustr_printfa (in_output, "    <padding2>\n");
    ustr_printfa (in_output, "    </padding2>\n");

    ustr_printfa (in_output, "  </head>\n");

    if (in_verbose)
    {
        int i;
        for (i = 0; i < in_hive->bin_cache->len; i++)
        {
            Bin *da_bin = (Bin*)g_ptr_array_index(in_hive->bin_cache, i);
            bin_get_xml_output(da_bin, in_output, TRUE);
        }
    }

    ustr_printfa (in_output, "<Hive>\n");

    return TRUE;
}

gboolean
hcheck_hive_pass(Hive *in_hive, RRACheckData *in_data)
{
    if (in_hive == NULL)
    {
        return FALSE;
    }

    if (!hive_is_valid(in_hive))
    {
        rra_check_warning(in_data, _("Hive not valid"));
        return FALSE;
    }

    gboolean ret_val = TRUE;

    rra_check_checking(in_data, _("checksum"));
    guint32 sum1 = generate_checksum(get_head(in_hive));

    if (get_head(in_hive)->checksum != sum1)
    {
        rra_check_error(in_data,
                        _("incorrect checksum (Correct: %d ; Existing: %d)"),
                        sum1, get_head(in_hive)->checksum);
        ret_val = FALSE;
    }

    rra_check_checking(in_data,
                       _("for 0s in unused part of header"));
    guint8 *header = offset_holder_get_header(in_hive->oh);
    guint i;

    for (i = sizeof(struct HiveHeader); i < REGF_HEADER_SIZE; i++)
    {
        if (header[i] != 0)
        {
            rra_check_error(in_data,
                            _("Byte %d in header not 0"), i);
            ret_val = FALSE;
        }
    }

    rra_check_checking(in_data, _("hive size validity"));

    guint32 lb = get_head(in_hive)->lastblock + HIVE_PAGE_LEN;
    guint32 hs = offset_holder_buf_size(in_hive->oh);
    if (hs != lb && (hs % HIVE_ALLOC_SIZE) != 0)
    {
        rra_check_error(in_data,
                        _("hive size is not a multiple of %#06x or equal "
                          "to lastblock: %d"), HIVE_ALLOC_SIZE, hs);
        ret_val = FALSE;
    }

    rra_check_checking(in_data,
                       _("that bins + trailer == hive size"));
    if (hs != (lb + hive_get_trailer_size(in_hive)))
    {
        rra_check_error(in_data,
                        _("bins + trailer (%d) != hive size (%d)"),
                        lb + hive_get_trailer_size(in_hive), hs);
        ret_val = FALSE;
    }

    rra_check_checking(in_data,
                       _("equivalent of last_bin and lastblock"));
    if ((offset_to_begin(bin_get_offset(hive_get_last_bin(in_hive)))
         + bin_size(hive_get_last_bin(in_hive)))
        != get_head(in_hive)->lastblock)
    {
        rra_check_error(
            in_data,
            _("lastblock (%#010x) != end of last bin (%#010x)"),
            get_head(in_hive)->lastblock,
            offset_to_begin(bin_get_offset(hive_get_last_bin(in_hive)))
            + bin_size(hive_get_last_bin(in_hive)));
        ret_val = FALSE;
    }

    rra_check_checking(in_data, _("for empty trailer"));
    offset trailer = hive_get_trailer_offset(in_hive);
    /* Is ok to get an invalid trailer. means no trailer. */
    if (offset_is_valid(trailer))
    {
        guint8 *tdata = offset_get_data(trailer);
        for (i = 0; i < hive_get_trailer_size(in_hive); i++)
        {
            if (tdata[i] != 0)
            {
                rra_check_warning(in_data, _("byte %d not 0 (%#04x)"),
                                  i, tdata[i]);
                /* ret_val = FALSE; */
            }
        }
    }

    return ret_val;
}
