/*
 * Authors:     Sean Loaring
 *              Petter Nordahl-Hagen
 *              James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006,2008 Racemi Inc
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

#include <ctype.h>
#include <string.h>

#include <glib/gi18n-lib.h>

#include <rregadmin/hive/key_list_cell.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/util/check.h>
#include <rregadmin/hive/hcheck_decl.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/hive/bin.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/log.h>
#include <rregadmin/util/binary_search.h>
#include <rregadmin/util/value_type.h>
#include <rregadmin/util/macros.h>

/** Common header for key list cells.
 *
 * @internal
 *
 * @ingroup klc_hive_group
 */
struct KeyListCellHeader
{
    int16_t no;
} RRA_VERBATIM_STRUCT;

/** Common data for key list cells.
 *
 * @internal
 *
 * @ingroup klc_hive_group
 */
struct KeyListCellData
{
    // id == ID_LF_KEY or ID_LH_KEY
    struct CellHeader cell_head;
    struct KeyListCellHeader head;
    guint8 data[];
} RRA_VERBATIM_STRUCT;

/** Object wrapper for key list cells.
 *
 * @ingroup klc_hive_group
 */
struct KeyListCell_
{
    Cell cell;
};


/** Hash value for lf_key.
 *
 * @internal
 *
 * @ingroup klc_hive_group
 */
struct lf_hash
{
    /* 0x0000   D-Word  Offset of corresponding "nk"-Record  */
    gint32 ofs_nk;
    /* 0x0004   D-Word  ASCII: the first 4 characters of the key-name,  */
    char name[4];
} RRA_VERBATIM_STRUCT;
/** This is the subkeylist/hash structure. NT4.0+.
 *
 * ID + count, then count number of offset/4byte "hash". (not true hash)
 * Probably changed from the 3.x version to make it faster to
 * traverse the registry if you're looking for a specific name
 * (saves lookups in 'nk's that have the first 4 name chars different)
 *
 * @internal
 *
 * @ingroup klc_hive_group
 */
struct lf_key
{
    struct KeyListCellHeader head;
    struct lf_hash hash[];
} RRA_VERBATIM_STRUCT;


/** Hash value for lh_key
 *
 * @internal
 *
 * @ingroup klc_hive_group
 */
struct lh_hash
{
    /* 0x0000 D-Word Offset of corresponding "nk"-Record  */
    gint32 ofs_nk;
    /* 0x0004 D-Word ASCII: the hash of the key name,  */
    gint32 hash;
} RRA_VERBATIM_STRUCT;
/** Subkey list/hash structure for WinXP and higher
 *
 * @internal
 *
 * @ingroup klc_hive_group
 */
struct lh_key
{
    struct KeyListCellHeader head;
    // WinXP uses a more real hash instead (base 37 of uppercase name
    // chars) padded with 0's. Case sensitive!
    struct lh_hash hash[];
} RRA_VERBATIM_STRUCT;


/** Hash value for li_key
 *
 * @internal
 *
 * @ingroup klc_hive_group
 */
struct li_hash
{
    /* 0x0000   D-Word  Offset of corresponding "nk"-Record  */
    gint32 ofs_nk;
} RRA_VERBATIM_STRUCT;

/** Subkey list/hash structure for Win 3.x
 *
 * Contains only offset table.
 *
 * @internal
 *
 * @ingroup klc_hive_group
 */
struct li_key
{
    struct KeyListCellHeader head;
    struct li_hash hash[];
} RRA_VERBATIM_STRUCT;

/** Offset to a KeyListCell.
 *
 * @see ri_key
 *
 * @internal
 *
 * @ingroup klc_hive_group
 */
typedef gint32 li_offset;
/** Subkey list/hash structure with two levels.
 *
 * This is a list of pointers to struct lh_key, ie
 * an extention record of many li's.
 *
 * This happens in NT4&5 when the lf hashlist grows larger
 * than about 400-500 entries/subkeys??, then the nk_key->ofs_lf points to this
 * instead of directly to an lf. (and it switches to li!)
 * Likely to happen in HKLM\\Software\\classes (file extention list) and
 * in SAM when many users.
 *
 * @internal
 *
 * @ingroup klc_hive_group
 */
struct ri_key
{
    struct KeyListCellHeader head;
    li_offset hash[];
} RRA_VERBATIM_STRUCT;

static gint32 get_hash_off (KeyListCell * lx, int off);

void * get_raw_slot (KeyListCell * lx, int off);

gboolean set_slot (KeyListCell * lx, int off, char *name, Cell *ptr);

static int cell_compare (KeyCell * lx, const char *name);
KeyListCell* set_appropriate_list(KeyCell *kc, KeyListCell *klc,
                                  int requested_count);

static int get_slot_size (guint16 type);

static int find_entry(const KeyListCell *klc, const char *name,
                      int *one_before);

static void debug_print_li_struct (struct li_key *key);
static void debug_print_lf_struct (struct lf_key *key);
static void debug_print_lh_struct (struct lh_key *key);
static void debug_print_ri_struct (struct ri_key *key);

static KeyListCell *ri_find_lx (KeyListCell *ri,
                                int off,
                                int *key_list_cell_off,
                                int *lxp_off);

static gboolean ri_drop_entry (Hive *hdesc, struct ri_key *ri, int no);
static KeyListCell* get_ri_entry(const KeyListCell *in_klc, int in_entry);

static gboolean lh_drop_entry (Hive *hdesc, struct lh_key *lh, int no);
static gint32 lh_hash (const char *name);

static gboolean li_drop_entry (Hive *hdesc, struct li_key *li, int no);

static void lf_hash (const char *name, char *buf);

static struct lh_key* get_lh(KeyListCell *klc);
static struct li_key* get_li(KeyListCell *klc);
static struct lf_key* get_lf(KeyListCell *klc);
static struct ri_key* get_ri(KeyListCell *klc);

static struct KeyListCellData* get_data(const KeyListCell *kc);
static Hive* get_hive(const KeyListCell *kc);
static offset get_offset(const KeyListCell *kc);

static inline struct KeyListCellData*
get_data(const KeyListCell *kc)
{
    return (struct KeyListCellData*)cell_get_data(key_list_cell_to_cell(
                                                      (KeyListCell*)kc));
}

static Hive*
get_hive(const KeyListCell *kc)
{
    return cell_get_hive(key_list_cell_to_cell((KeyListCell*)kc));
}

static offset
get_offset(const KeyListCell *kc)
{
    return cell_get_offset(key_list_cell_to_cell((KeyListCell*)kc));
}


static struct li_key*
get_li(KeyListCell *klc)
{
    return (struct li_key*)&get_data(klc)->head;
}

static struct lf_key*
get_lf(KeyListCell *klc)
{
    return (struct lf_key*)&get_data(klc)->head;
}

static struct ri_key*
get_ri(KeyListCell *klc)
{
    return (struct ri_key*)&get_data(klc)->head;
}

static struct lh_key*
get_lh(KeyListCell *klc)
{
    return (struct lh_key*)&get_data(klc)->head;
}

/*********************************************************************/

KeyListCell *
key_list_cell_alloc (Hive * hdesc, offset p, int id, int count)
{
    Cell *hb;
    KeyListCell *result;
    int newsize = 0;

    int alloc_count = ((count > 0) ? count : 1);

    rra_debug("Allocing");

    newsize = (sizeof (struct KeyListCellHeader)
               + (get_slot_size(id) * alloc_count));

    hb = cell_alloc (hdesc, p, newsize);
    if (hb == NULL)
    {
        rra_warning(N_("hb == NULL"));
        return NULL;
    }

    result = (KeyListCell *) hb;

    get_data(result)->head.no = 0;
    get_data(result)->cell_head.id = id;

    return result;
}

KeyListCell*
key_list_cell_alloc_copy (Hive *hdesc, const KeyListCell *in_klc, int count)
{
    KeyListCell *ret_val;
    int copy_amount;

    rra_debug("Allocing copy from %#010x",
              offset_to_begin(get_offset(in_klc)));

    if ((int)key_list_cell_get_capacity(in_klc) >= count)
    {
        rra_warning(N_("Asked to alloc copy into same or smaller size"));
        return NULL;
    }

    ret_val = key_list_cell_alloc(hdesc, get_offset((KeyListCell*)in_klc),
                                  key_list_cell_get_type(in_klc),
                                  count);

    if (ret_val == NULL)
    {
        rra_warning(N_("Unable to allocate new copy of KeyListCell"));
        return NULL;
    }

    copy_amount = key_list_cell_get_size(in_klc)
        * get_slot_size(key_list_cell_get_type(in_klc));

    memcpy(get_data(ret_val)->data, get_data(in_klc)->data, copy_amount);
    get_data(ret_val)->head.no = get_data(in_klc)->head.no;
    get_data(ret_val)->cell_head.id = get_data(in_klc)->cell_head.id;

    return ret_val;
}

gboolean
key_list_cell_unalloc(KeyListCell *klc, gboolean in_recursive)
{
    rra_info(N_("Unallocating (%#010x): Recursive=%s"),
             offset_to_begin(cell_get_offset(key_list_cell_to_cell(klc))),
             in_recursive ? N_("true") : N_("false"));
    if (in_recursive)
    {
        int i;
        for (i = 0; i < key_list_cell_get_size(klc); i++)
        {
            key_cell_unalloc(key_list_cell_get_entry(klc, i), in_recursive);
        }
    }

    return cell_unalloc(key_list_cell_to_cell(klc));
}

KeyListCell *
key_list_cell_from_cell (Cell *p)
{
    KeyListCell *result = (KeyListCell*)p;

    if (key_list_cell_is_valid(result))
    {
        return result;
    }
    else
    {
        return NULL;
    }
}

Cell *
key_list_cell_to_cell (KeyListCell * lx)
{
    return (Cell *) lx;
}

gboolean
key_list_cell_is_valid (const KeyListCell *klc)
{
    guint16 type = key_list_cell_get_type(klc);

    if (type != ID_LF_KEY && type != ID_LH_KEY
        && type != ID_LI_KEY && type != ID_RI_KEY)
    {
        rra_message(N_("KeyListCell had invalid type %d (%s)"),
                    type, value_type_get_string(type));
        return FALSE;
    }

    return TRUE;
}

guint32
key_list_cell_get_number_of_sublists(const KeyListCell *klc)
{
    if (key_list_cell_get_type(klc) == ID_RI_KEY)
    {
        struct ri_key *k = get_ri((KeyListCell*)klc);
        return k->head.no;
    }
    else
    {
        return 0;
    }
}

KeyListCell*
key_list_cell_get_sublist(KeyListCell *klc, guint32 in_entry)
{
    return get_ri_entry(klc, in_entry);
}

/***********************************************************************/

guint32
key_list_cell_get_size(const KeyListCell *klc)
{
    if (key_list_cell_get_type(klc) == ID_RI_KEY)
    {
        int i;
        int size = 0;
        for (i = 0; i < get_data(klc)->head.no; i++)
        {
            size += key_list_cell_get_size(get_ri_entry(klc, i));
        }

        return size;
    }
    else
    {
        return get_data(klc)->head.no;
    }
}

guint32
key_list_cell_get_capacity(const KeyListCell *klc)
{
    if (key_list_cell_get_type(klc) == ID_RI_KEY)
    {
        int i;
        int capacity = 0;
        for (i = 0; i < get_data(klc)->head.no; i++)
        {
            capacity += key_list_cell_get_capacity(get_ri_entry(klc, i));
        }

        return capacity;
    }
    else
    {
        int csize = cell_size(key_list_cell_to_cell((KeyListCell*)klc));
        // remove header sizes
        csize -= sizeof(struct CellHeader);
        csize -= sizeof(struct KeyListCellHeader);
        // The rest is an array of hashes.
        return csize / get_slot_size(key_list_cell_get_type(klc));
    }
}

guint16
key_list_cell_get_type(const KeyListCell *klc)
{
    return get_data(klc)->cell_head.id;
}

static int
get_slot_size (guint16 type)
{
    switch (type)
    {
    case ID_LI_KEY:
        return sizeof (struct li_hash);
    case ID_LF_KEY:
        return sizeof (struct lf_hash);
    case ID_LH_KEY:
        return sizeof (struct lh_hash);
    case ID_RI_KEY:
        return sizeof (li_offset);
    default:
        rra_warning(N_("Unknown key list type: %#06x"), type);
        return 0;
    }
}


/***********************************************************************/

// return the offset to a slot in the array
void *
get_raw_slot (KeyListCell * lx, int off)
{
    if (off < 0 || off > (int)key_list_cell_get_size(lx))
    {
        rra_warning(N_("Invalid offset into KeyListCell: %d (size=%d)"),
                    off, key_list_cell_get_size(lx));
        return NULL;
    }

    switch (key_list_cell_get_type(lx))
    {
    case ID_LI_KEY:
        return &get_li(lx)->hash[off];

    case ID_LF_KEY:
        return &get_lf(lx)->hash[off];

    case ID_LH_KEY:
        return &get_lh(lx)->hash[off];

    case ID_RI_KEY:
        return &get_ri(lx)->hash[off];

    default:
        rra_warning(N_("Unknown key list type: %#06x"),
                    key_list_cell_get_type(lx));
        return NULL;
    }
}

static gint32
get_hash_off (KeyListCell * lx, int off)
{
    switch (key_list_cell_get_type(lx))
    {
    case ID_LI_KEY:
        return get_li(lx)->hash[off].ofs_nk;
    case ID_LF_KEY:
        return get_lf(lx)->hash[off].ofs_nk;
    case ID_LH_KEY:
        return get_lh(lx)->hash[off].ofs_nk;
    case ID_RI_KEY:
        rra_warning (_("This function inappropriate for RI key lists"));
        return -1;
    default:
        rra_warning(N_("Unknown key list type: %#06x"),
                    key_list_cell_get_type(lx));
        return -1;
    }
}

/*******************************************************************/

static int
cell_compare (KeyCell *k_cell, const char *name)
{
    ustring *key_name = key_cell_get_name (k_cell);
    ustring *name_ustr = ustr_create(name);
    int result = ustr_regcmp (name_ustr, key_name);
    rra_debug("Comparing %s and %s => %d",
              name, ustr_as_utf8(key_name), result);
    ustr_free(key_name);
    ustr_free(name_ustr);
    return result;
}

static int
find_entry(const KeyListCell *klc, const char *name, int *one_before)
{
    return rra_binary_search(0, key_list_cell_get_size (klc) - 1,
                             name, klc,
                             (RRARetrieveFunc)key_list_cell_get_entry,
                             (GCompareFunc)cell_compare,
                             one_before);
}

/******************************************************************/

KeyCell*
key_list_cell_get_entry_str (const KeyListCell * lx, const char *name)
{
    return key_list_cell_get_entry(lx, find_entry(lx, name, 0));
}

KeyCell *
key_list_cell_get_entry (const KeyListCell * lx, guint32 off)
{
    if (off == -1)
    {
        return NULL;
    }

    rra_debug("Looking for entry %d out of %d in %#010x",
              off, key_list_cell_get_size(lx),
              offset_to_begin(get_offset(lx)));

    switch (key_list_cell_get_type(lx))
    {
    case ID_RI_KEY:
        {
            KeyListCell *lxs;
            int lxs_off;
            int lxp_off;

            lxs = ri_find_lx ((KeyListCell*)lx, off, &lxs_off, &lxp_off);

            if (lxs == NULL)
            {
                return NULL;
            }

            return key_list_cell_get_entry (lxs, lxs_off);
        }

    case ID_LF_KEY:
    case ID_LI_KEY:
    case ID_LH_KEY:
        {
            int size = key_list_cell_get_size(lx);

            if (size <= 0 || off >= size)
            {
                return NULL;
            }
            else
            {
                KeyListCell *tmp_klc = (KeyListCell*)lx;
                return key_cell_from_cell(
                    hive_get_cell_from_offset (
                        get_hive(lx), get_hash_off(tmp_klc, off)));
            }
        }

    default:
        rra_warning(N_("Unknown key list type: %#06x"),
                    key_list_cell_get_type(lx));
        return NULL;
    }
}

/********************************************************************/

gboolean
key_list_cell_delete_entry_str (KeyListCell * lx, const char *name)
{
    return key_list_cell_delete_entry(lx, find_entry(lx, name, 0));
}

// Removes an entry from an lx
gboolean
key_list_cell_delete_entry (KeyListCell * lx, guint32 in_index)
{
    if (in_index == -1)
    {
        rra_message(N_("Bad entry number: -1"));
        return FALSE;
    }

    switch (key_list_cell_get_type(lx))
    {
    case ID_LF_KEY:
    case ID_LH_KEY:
        {
            return lh_drop_entry (get_hive(lx), get_lh(lx), in_index);
        }

    case ID_RI_KEY:
        {
            int lxs_no;
            int lxp_off;

            KeyListCell *lxs = ri_find_lx (lx, in_index, &lxs_no, &lxp_off);
            if (lxs == NULL)
            {
                rra_message(N_("Couldn't find lx entry in ri for %d"),
                            in_index);
                return FALSE;
            }

            if (key_list_cell_delete_entry (lxs, lxs_no))
            {
                if (get_data(lxs)->head.no == 0)
                {
                    ri_drop_entry (get_hive(lx), get_ri(lx), lxp_off);
                }
                return TRUE;
            }
            else
            {
                rra_message(N_("Failed to drop entry %d from sublist"),
                            lxs_no);
                return FALSE;
            }
        }

    case ID_LI_KEY:
        {
            return li_drop_entry (get_hive(lx), get_li(lx), in_index);
        }

    default:
        rra_warning(N_("Unknown key list type: %#06x"),
                    key_list_cell_get_type(lx));
        return FALSE;
    }
}

/******************************************************************/

gboolean
set_slot (KeyListCell * lx, int off, char *name, Cell *ptr)
{
    int ptr_ofs = offset_to_begin(cell_get_offset(ptr));

    switch (key_list_cell_get_type(lx))
    {
    case ID_LI_KEY:
        get_li(lx)->hash[off].ofs_nk = ptr_ofs;
        return TRUE;

    case ID_LF_KEY:
        lf_hash (name, get_lf(lx)->hash[off].name);
        get_lf(lx)->hash[off].ofs_nk = ptr_ofs;
        return TRUE;

    case ID_LH_KEY:
        get_lh(lx)->hash[off].hash = lh_hash (name);
        get_lh(lx)->hash[off].ofs_nk = ptr_ofs;
        return TRUE;

    case ID_RI_KEY:
        rra_warning(N_("This function inappropriate for RI key lists"));
        return FALSE;

    default:
        rra_warning(N_("Unknown key list type: %#06x"),
                    key_list_cell_get_type(lx));
        return FALSE;
    }
}

KeyListCell*
set_appropriate_list(KeyCell *kc, KeyListCell *klc, int requested_count)
{
    KeyListCell *ret_val;
    Hive *hv;
    const HiveVersion *ver;
    int type = ID_LH_KEY;
    int default_count = ((requested_count > 5) ? requested_count * 2 : 10);

    hv = cell_get_hive(key_cell_to_cell(kc));
    ver = hive_get_version(hv);

    if (klc != NULL)
    {
        ret_val = key_list_cell_alloc_copy(hv, klc, default_count);
    }
    else
    {
        if (ver->major == 1)
        {
            if (ver->minor == 3)
            {
                type = ID_LF_KEY;
            }
            else if (ver->minor == 5)
            {
                type = ID_LH_KEY;
            }
            else
            {
                rra_warning(N_("Unknown hive version: %d.%d.%d.%d"),
                            ver->major, ver->minor, ver->release, ver->build);
            }
        }
        else
        {
            rra_warning(N_("Unknown hive version: %d.%d.%d.%d"),
                        ver->major, ver->minor, ver->release, ver->build);
        }
        ret_val = key_list_cell_alloc(
            hv, cell_get_offset(key_cell_to_cell(kc)), type, default_count);
    }

    key_cell_set_key_list_cell(kc, ret_val, FALSE);

    return ret_val;
}

KeyListCellAddReturn
key_list_cell_add_entry (KeyListCell * lx, KeyCell *nkparent, KeyCell *nkchild)
{
    ustring *name;
    int location;
    int one_before;

    if (lx == NULL)
    {
        KeyListCell *klc = set_appropriate_list(nkparent, 0, 0);
        if (klc != NULL)
        {
            return key_list_cell_add_entry(klc, nkparent, nkchild);
        }
        else
        {
            return KLC_ADD_ERROR;
        }
    }

    name = key_cell_get_name(nkchild);
    location = find_entry(lx, ustr_as_utf8(name), &one_before);
    ustr_free(name);
    name = 0;

    if (key_list_cell_get_type(lx) == ID_RI_KEY)
    {
        int key_list_cell_off;
        int ri_bucket_off;
        int to_find;

        KeyListCell *slx;

        to_find = ((location == -1) ? one_before : location);

        slx = ri_find_lx (lx, to_find, &key_list_cell_off, &ri_bucket_off);

        if (slx == NULL)
        {
            rra_warning(
                _("Couldn't find lx key list cell from ri key list cell"));
            return KLC_ADD_ERROR;
        }

        gint sub_ret = key_list_cell_add_entry (slx, nkparent, nkchild);

        return sub_ret;
    }
    else
    {
        gboolean ret_val;
        KeyListCell *da_klc = lx;

        int offset = one_before + 1;

        if (key_list_cell_get_size(lx) + 1
            > key_list_cell_get_capacity(lx))
        {
            da_klc = set_appropriate_list(
                nkparent, lx, key_list_cell_get_capacity(lx) + 1);
        }

        if (location == -1)
        {
            if (key_list_cell_get_size(da_klc) - offset > 0)
            {
                void *dest = get_raw_slot (da_klc, offset + 1);
                void *src = get_raw_slot (da_klc, offset);
                if (dest != NULL && src != NULL)
                {
                    if (get_data(da_klc)->head.no > offset)
                    {
                        memmove (
                            dest, src,
                            (get_slot_size(key_list_cell_get_type(da_klc))
                             * (get_data(da_klc)->head.no - offset)));
                    }
                }
                else
                {
                    rra_warning(
                        _("Got NULL for dest (%p) or src (%p) to copy"),
                        dest, src);
                }
            }
        }

        name = key_cell_get_name (nkchild);
        ret_val = set_slot (da_klc, offset, (char*)ustr_as_utf8(name),
                            key_cell_to_cell (nkchild));

        ustr_free(name);

        if (ret_val)
        {
            if (location == -1)
            {
                get_data(da_klc)->head.no++;
                return KLC_ADD_ADDED;
            }
            else
            {
                return KLC_ADD_REPLACED;
            }
        }
        else
        {
            return KLC_ADD_ERROR;
        }
    }
}

/*******************************************************************/

static gboolean
ri_drop_entry (Hive *hdesc, struct ri_key *ri, int no)
{
    Cell *child;
    if (no < 0 || no >= ri->head.no)
    {
        return FALSE;
    }

    child = hive_get_cell_from_offset (hdesc, ri->hash[no]);

    if (ri->head.no - 1 > no)
    {
        //TODO; hash sizes are assumed the same
        memmove (&ri->hash[no],
                 &ri->hash[no + 1],
                 sizeof (li_offset) * (ri->head.no - (no + 1)));
    }

    ri->head.no--;

    key_cell_unalloc (key_cell_from_cell(child), TRUE);
    return TRUE;
}

static KeyListCell*
get_ri_entry(const KeyListCell *in_klc, int in_entry)
{
    if (in_entry < get_data(in_klc)->head.no)
    {
        return key_list_cell_from_cell (
            hive_get_cell_from_offset (
                get_hive(in_klc),
                get_ri((KeyListCell*)in_klc)->hash[in_entry]));
    }
    else
    {
        return NULL;
    }
}

static KeyListCell *
ri_find_lx (KeyListCell *in_klc, int off, int *key_list_cell_off, int *lxp_off)
{
    int roff = 0;
    int cntr = 0;
    for (roff = 0; roff < get_data(in_klc)->head.no; roff++)
    {
        KeyListCell *lx = get_ri_entry(in_klc, roff);

        if (cntr + get_data(lx)->head.no > off)
        {
            if (key_list_cell_off)
                *key_list_cell_off = off - cntr;
            if (lxp_off)
                *lxp_off = roff;
            return lx;
        }
        cntr += get_data(lx)->head.no;
    }

    return NULL;
}

/********************************************************************/

static gboolean
li_drop_entry (Hive *hdesc, struct li_key *li, int no)
{
    Cell *child;

    if (no >= li->head.no)
    {
        return FALSE;
    }

    child = hive_get_cell_from_offset (hdesc, li->hash[no].ofs_nk);

    if (li->head.no - 1 > no)
    {
        //TODO; hash sizes are assumed the same
        memmove (&li->hash[no],
                 &li->hash[no + 1],
                 sizeof (struct li_hash) * (li->head.no - (no + 1)));
    }

    li->head.no--;

    key_cell_unalloc (key_cell_from_cell(child), TRUE);

    return TRUE;
}

/********************************************************************/

static gboolean
lh_drop_entry (Hive *hdesc, struct lh_key *lh, int no)
{
    Cell *child;

    if (no >= lh->head.no)
    {
        return FALSE;
    }

    child = hive_get_cell_from_offset (hdesc, lh->hash[no].ofs_nk);

    if (lh->head.no - 1 > no)
    {
        //TODO; hash sizes are assumed the same
        memmove (&lh->hash[no],
                 &lh->hash[no + 1],
                 sizeof (struct lh_hash) * (lh->head.no - (no + 1)));
    }

    lh->head.no--;

    key_cell_unalloc (key_cell_from_cell(child), TRUE);

    return TRUE;
}

// Generate an lh-style hash
static gint32
lh_hash (const char *name)
{
    guint32 i = 0;
    gint32 hash = 0;

    for (i = 0; i < strlen (name); i++)
    {
        hash *= 37;
        hash += toupper (name[i]);
    }

    return hash;
}

/********************************************************************/

static void
lf_hash (const char *name, char *buf)
{
    memset (buf, 0, 4);
    strncpy (buf, name, 4);
}


/********************************************************************/

static void
debug_print_lf_struct (struct lf_key *key)
{
    int i;

    fprintf (stderr, "  <lf>\n");

    fprintf (stderr, "    <offsets>\n");
    for (i = 0; i < key->head.no; i++)
    {
        fprintf (stderr, "      <offset value=\"%#010x\" hash=\"%c%c%c%c\"/>\n",
                 key->hash[i].ofs_nk,
                 key->hash[i].name[0],
                 key->hash[i].name[1],
                 key->hash[i].name[2],
                 key->hash[i].name[3]);
    }
    fprintf (stderr, "    </offsets>\n");

    fprintf (stderr, "  </lf>\n");
}

static void
debug_print_lh_struct (struct lh_key *lh)
{
    /*
     * The hash is most likely a base 37 conversion of the name string
     */

    int i;
    fprintf (stderr, "  <lh>\n");

    fprintf (stderr, "    <offsets>\n");
    for (i = 0; i < lh->head.no; i++)
    {
        fprintf (stderr, "      <offset value=\"%#010x\" hash=\"%#010x\"/>\n",
                 lh->hash[i].ofs_nk, lh->hash[i].hash);
    }
    fprintf (stderr, "    </offsets>\n");

    fprintf (stderr, "  </lh>\n");
}

static void
debug_print_li_struct (struct li_key *key)
{
    int i;

    fprintf (stderr, "  <li>\n");

    fprintf (stderr, "    <offsets>\n");
    for (i = 0; i < key->head.no; i++)
    {
        fprintf (stderr, "      <offset value=\"%#010x\"/>\n",
                 key->hash[i].ofs_nk);
    }
    fprintf (stderr, "    </offsets>\n");

    fprintf (stderr, "  </li>\n");
}

static void
debug_print_ri_struct (struct ri_key *key)
{
    int i;

    fprintf (stderr, "  <ri>\n");

    fprintf (stderr, "    <offsets>\n");
    for (i = 0; i < key->head.no; i++)
    {
        fprintf (stderr, "      <li_offset value=\"%#010x\"/>\n",
                 key->hash[i]);
    }
    fprintf (stderr, "    </offsets>\n");

    fprintf (stderr, "  </li>\n");
}


void
key_list_cell_debug_print (KeyListCell * lxkey)
{
    fprintf(stderr, "<KeyListCell offset=\"%#010x\" size=\"%d\">\n",
            offset_to_begin(get_offset(lxkey)),
            get_data(lxkey)->cell_head.size);

    fprintf (stderr, "  <size value=\"%d\"/>\n",
             key_list_cell_get_size(lxkey));
    fprintf (stderr, "  <capacity value=\"%d\"/>\n",
             key_list_cell_get_capacity(lxkey));
    const char *hs_format_str =
        "  <header_size value=\"%" G_GSIZE_MODIFIER "\"/>\n";
    fprintf (stderr, hs_format_str,
             sizeof(struct CellHeader) + sizeof(struct KeyListCellHeader));
    fprintf (stderr, "  <slot_size value=\"%d\"/>\n",
             get_slot_size(key_list_cell_get_type(lxkey)));

    switch (key_list_cell_get_type(lxkey))
    {
    case ID_LF_KEY:
        debug_print_lf_struct (get_lf(lxkey));
        break;
    case ID_LI_KEY:
        debug_print_li_struct (get_li(lxkey));
        break;
    case ID_LH_KEY:
        debug_print_lh_struct (get_lh(lxkey));
        break;
    case ID_RI_KEY:
        debug_print_ri_struct (get_ri(lxkey));
        break;
    default:
        rra_warning("Unknown key list type: %#06x",
                    key_list_cell_get_type(lxkey));
        break;
    }

    fprintf(stderr, "</KeyListCell>\n");
}

static gboolean
walk_tree_checker(KeyCell *in_kc, RRACheckData *in_data)
{
    if (in_kc == NULL)
    {
        rra_check_error(in_data, _("NULL key cell"));
        return FALSE;
    }

    KeyListCell *klc = key_cell_get_key_list_cell(in_kc);

    if (klc == NULL)
    {
        /* It's ok for one not to exist */
        return TRUE;
    }

    if (!key_list_cell_is_valid(klc))
    {
        rra_check_error(in_data, _("Invalid key list cell"));
        return FALSE;
    }
    gboolean ret_val = TRUE;

    const HiveVersion *ver =
        hive_get_version(cell_get_hive(key_cell_to_cell(in_kc)));

    rra_check_checking(in_data, _("for a valid type"));
    guint16 type = key_list_cell_get_type(klc);
    if (ver->major == 1)
    {
        if (ver->minor == 3)
        {
            if (type != ID_LF_KEY
                && type != ID_RI_KEY)
            {
                rra_check_error(in_data, _("invalid type for v 1.3"));
                ret_val = FALSE;
            }
        }
        else if (ver->minor == 5)
        {
            if (type != ID_LH_KEY
                && type != ID_RI_KEY)
            {
                rra_check_error(in_data, _("invalid type for v 1.5"));
                ret_val = FALSE;
            }
        }
        else
        {
            rra_check_error(in_data,
                            _("don't know proper type for v %d.%d"),
                            ver->major, ver->minor);
            ret_val = FALSE;
        }
    }
    else
    {
        rra_check_error(in_data,
                        _("don't know proper type for v %d.%d"),
                        ver->major, ver->minor);
        ret_val = FALSE;
    }

    rra_check_checking(in_data, _("for size capacity matchup"));
    if (key_list_cell_get_capacity(klc) < key_list_cell_get_size(klc))
    {
        rra_check_error(in_data,
                        _("KeyListCell capacities mismatch (%d vs %d)"),
                        key_list_cell_get_capacity(klc),
                        key_list_cell_get_size(klc));
        ret_val = FALSE;
    }

    rra_check_checking(in_data,
                       _("for key cell / key list cell count equality"));
    if (key_cell_get_number_of_subkeys(in_kc)
        != key_list_cell_get_size(klc))
    {
        rra_check_error(
            in_data,
            _("counts wrong: %d (key cell) vs %d (key list cell)"),
            key_cell_get_number_of_subkeys(in_kc),
            key_list_cell_get_size(klc));
        ret_val = FALSE;
    }

    if (key_list_cell_get_size(klc) > 1)
    {
        rra_check_checking(in_data,
                           _("for correct ordering among %d keys"),
                           key_cell_get_number_of_subkeys(in_kc));
        guint32 i = 0;
        for (i = 0; i < key_list_cell_get_size(klc) - 1; i++)
        {
            KeyCell *kc1 = key_list_cell_get_entry((KeyListCell*)klc, i);
            KeyCell *kc2 = key_list_cell_get_entry((KeyListCell*)klc, i + 1);

            ustring *kc2_name = key_cell_get_name(kc2);

            if (cell_compare(kc1, ustr_as_utf8(kc2_name)) <= 0)
            {
                ustring *kc1_name = key_cell_get_name(kc1);
                rra_check_error(
                    in_data,
                    _("KeyCell ordering messed up.  %s before %s"),
                    ustr_as_utf8(kc1_name),
                    ustr_as_utf8(kc2_name));
                ustr_free(kc1_name);
                ustr_free(kc2_name);
                ret_val =  FALSE;

            }

            ustr_free(kc2_name);
        }
    }

    int i = 0;
    for (i = 0; i < key_cell_get_number_of_subkeys(in_kc); i++)
    {
        KeyCell *subk = key_cell_get_subkey(in_kc, i);
        if (!walk_tree_checker(subk, in_data))
        {
            ret_val = FALSE;
        }
    }

    return ret_val;
}

gboolean
hcheck_key_list_cell_pass(Hive *in_hive, RRACheckData *in_data)
{
    if (in_hive == NULL)
    {
        return FALSE;
    }

    gboolean ret_val = TRUE;

    KeyCell *kc = hive_get_root_key(in_hive);
    if (!walk_tree_checker(kc, in_data))
    {
        ret_val = FALSE;
    }

    return ret_val;
}
