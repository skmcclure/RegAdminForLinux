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

#include <errno.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/hive/cell.h>
#include <rregadmin/hive/value_cell.h>
#include <rregadmin/hive/bin.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/key_list_cell.h>
#include <rregadmin/hive/security_descriptor_cell.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/hive/value_list_cell.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/util/check.h>
#include <rregadmin/hive/hcheck_decl.h>
#include <rregadmin/hive/log.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/util/giochan_helpers.h>

/** Object wrapper for hive key cells.
 *
 * @ingroup kc_hive_group
 */
struct KeyCell_
{
    Cell cell;
};

/** This is the key node (ie directory) descriptor, can contain subkeys
 * and/or values.
 *
 * \note for values, the count is stored here, but for subkeys
 * there is a count both here and in the offset-table (lf or li struct).
 *
 * What happens if these mismatch is not known.
 *
 * What's the classname thingy? Can't remember seeing that used in
 * anything I've looked at.
 *
 * @internal
 *
 * @ingroup kc_hive_group
 */
struct KeyCellData
{
    /** header.
     *
     * The id will be "nk" = 0x6B6E
     */
    struct CellHeader cell_head;
    /** Type of the key.
     *
     * @see KeyCellType
     */
    key_cell_type type;
    /** Last modified time.
     */
    NTTIME timestamp;
    /** Not sure what this is.
     */
    guint32 unknown1;
    /** Offset of parent key
     */
    gint32 ofs_parent;
    /** number of subkeys
     */
    guint32 no_subkeys;
    /** Not sure what this is.
     */
    guint32 unknown2;
    /** Offset of the KeyListCell
     */
    gint32 ofs_lf;
    /** Not sure what this is.
     */
    guint32 ofs_unknown1;
    /*  Number of values
     */
    guint32 no_values;
    /*  Offset of the ValueListCell
     */
    gint32 ofs_vallist;
    /*  Offset of the SecurityDescriptorCell
     */
    guint32 ofs_sk;
    /** Offset of the classname
     *
     * Is this a value cell?  I don't really know.
     */
    gint32 ofs_classname;
    /** Not sure what this is.
     */
    guint32 unknown3[4];
    /** Not sure what this is.
     */
    guint32 unknown4;
    /** The length of the name appended to the cell
     */
    guint16 len_name;
    /** The length of the classname
     */
    guint16 len_classname;
    /** Variable sized array to contain the name
     */
    guint8 keyname[];
} RRA_VERBATIM_STRUCT;


static gboolean key_cell_set_dirty (KeyCell *in_kc);
static gint32 key_cell_find_value (KeyCell *in_kc, const char *in_name);
static void key_cell_check_nullify_key_list_cell(KeyCell *in_kc);

static struct KeyCellData* get_data(KeyCell *in_kc);
static Hive* get_hive(KeyCell *in_kc);
static offset get_offset(KeyCell *in_kc);

static inline struct KeyCellData*
get_data(KeyCell *in_kc)
{
    return (struct KeyCellData*)cell_get_data(key_cell_to_cell(in_kc));
}

static Hive*
get_hive(KeyCell *in_kc)
{
    return cell_get_hive(key_cell_to_cell(in_kc));
}

static offset
get_offset(KeyCell *in_kc)
{
    return cell_get_offset(key_cell_to_cell(in_kc));
}

// Allocate a new key_cell_key in same page as p, if possible
KeyCell *
key_cell_alloc (Hive *in_hive, offset in_ofs, const ustring *in_name)
{
    KeyCell *result;
    Cell *new_cell;

    new_cell = cell_alloc (in_hive, in_ofs,
                           sizeof (struct KeyCellData) + ustr_strlen(in_name)
                           /* cell_alloc will include this size */
                           - sizeof(struct CellHeader));

    if (new_cell == NULL)
    {
        return NULL;
    }

    result = (KeyCell*)new_cell;

    //populate it
    get_data(result)->cell_head.id = ID_NK_KEY;
    get_data(result)->type = KEY_NORMAL;
    //hive_heap_offset( hdesc, nkparent );
    //TODO: update this on insertion
    get_data(result)->ofs_parent = 0;
    get_data(result)->no_subkeys = 0;
    get_data(result)->ofs_lf = -1;
    get_data(result)->no_values = 0;
    get_data(result)->ofs_vallist = -1;
    get_data(result)->ofs_sk = 0;
    get_data(result)->ofs_classname = -1;
    get_data(result)->len_name = ustr_strlen (in_name);
    get_data(result)->len_classname = 0;
    get_data(result)->timestamp = nttime_now();

    strncpy ((char*)get_data(result)->keyname,
             ustr_as_utf8(in_name),
             ustr_strlen(in_name));

    return result;
}

KeyCell *
key_cell_alloc_root (Hive *in_hive)
{
    ustring *name = ustr_create("$$$PROTO.HIV");
    KeyCell *ret_val =
        key_cell_alloc(in_hive,
                       bin_get_offset(hive_get_first_bin(in_hive)), name);

    ustr_free(name);

    if (ret_val == NULL)
    {
        return NULL;
    }

    const char *sddl = "O:S-1-83886080-32-544"
        "G:S-1-83886080-18"
        "D:(A;;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-18)"
        "(A;;RCRPCCSW;;;S-1-83886080-32-544)";
    const char *sddl_tmp = sddl;

    SecurityDescriptor *sd = secdesc_new_parse_sddl(&sddl_tmp);
    if (sd == NULL)
    {
        rra_warning("Unable to create SecurityDescriptor from %s", sddl);
        return NULL;
    }

    if (!key_cell_set_secdesc(ret_val, sd))
    {
        rra_warning("Unable to set root cell's security descriptor");
        return NULL;
    }

    get_data(ret_val)->type = KEY_ROOT;
    return ret_val;
}

gboolean
key_cell_unalloc(KeyCell *in_kc, gboolean in_recursive)
{
    rra_info(N_("Unallocating (%#010x): Recursive=%s"),
             offset_to_begin(cell_get_offset(key_cell_to_cell(in_kc))),
             in_recursive ? N_("true") : N_("false"));
    if (in_recursive)
    {
        ValueListCell *vlc;
        KeyListCell *klc = key_cell_get_key_list_cell(in_kc);
        if (klc != NULL)
        {
            key_list_cell_unalloc(klc, TRUE);
        }

        vlc = key_cell_get_value_list_cell(in_kc);
        if (vlc != NULL)
        {
            value_list_cell_unalloc(vlc,
                                    key_cell_get_number_of_values(in_kc),
                                    TRUE);
        }

        if (get_data(in_kc)->len_classname > 0)
        {
            Cell *cell = hive_get_cell_from_offset(
                get_hive(in_kc), get_data(in_kc)->ofs_classname);
            if (cell != NULL)
            {
                ValueCell *vcell = value_cell_from_cell(cell);
                value_cell_unalloc(vcell);
            }
        }
    }

    security_descriptor_cell_decrement_use_count(
        key_cell_get_security_descriptor_cell(in_kc));

    return cell_unalloc(key_cell_to_cell(in_kc));
}

KeyCell *
key_cell_from_cell (Cell *in_cell)
{
    KeyCell *result = (KeyCell*)in_cell;

    return result;
}

Cell *
key_cell_to_cell (KeyCell *in_kc)
{
    return (Cell*)in_kc;
}

#define INVALID_KEYCELL_MSG "Invalid KeyCell: "

// returns: 1 == good, 0 == bad
gboolean
key_cell_is_valid (KeyCell *in_kc)
{
    /* id ok */
    if (get_data(in_kc)->cell_head.id != ID_NK_KEY)
    {
        rra_message(N_(INVALID_KEYCELL_MSG "id"));
        return FALSE;
    }

    return TRUE;
}

static gboolean
key_cell_set_dirty (KeyCell *in_kc)
{
    get_data(in_kc)->timestamp = nttime_now();
    return TRUE;
}

key_cell_type
key_cell_get_type_id(KeyCell *in_kc)
{
    return get_data(in_kc)->type;
}

const char *
key_cell_get_type_str(KeyCell *in_kc)
{
    int type_id = key_cell_get_type_id(in_kc);

    if (type_id == KEY_ROOT)
    {
        return "root";
    }
    else if (type_id == KEY_NORMAL)
    {
        return "normal";
    }
    else if (type_id == KEY_LINK)
    {
        return "link";
    }
    else
    {
        return "unknown";
    }
}

ustring*
key_cell_get_name (KeyCell *in_kc)
{
    ustring *ret_val;

    ret_val = ustr_new();
    if (!ustr_strnset(ret_val, (char*)get_data(in_kc)->keyname,
                      get_data(in_kc)->len_name))
    {
        if (!ustr_strnset_type(ret_val, USTR_TYPE_ISO8859_1,
                               (char*)get_data(in_kc)->keyname,
                               get_data(in_kc)->len_name))
        {
            rra_warning(N_("Couldn't convert key cell's name"));
        }
    }

    return ret_val;
}

ustring*
key_cell_get_classname (KeyCell *in_kc)
{
    ValueCell *clsnm_cell = key_cell_get_classname_value_cell(in_kc);
    ustring *ret_val = ustr_new();

    if (clsnm_cell != NULL)
    {
        ustr_strnset_type(ret_val, USTR_TYPE_UTF16LE,
                          (const char*)value_cell_get_data(clsnm_cell),
                          get_data(in_kc)->len_classname);
    }

    return ret_val;
}

int
key_cell_compare_names(KeyCell *in_kc1, KeyCell *in_kc2)
{
    if (in_kc1 == NULL)
    {
        if (in_kc2 == NULL)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        if (in_kc1 == NULL)
        {
            return 1;
        }
    }

    int len_check = MIN(get_data(in_kc1)->len_name,
                        get_data(in_kc2)->len_name);
    int i;
    for (i = 0; i < len_check; i++)
    {
        int diff = toupper(get_data(in_kc1)->keyname[i])
            - toupper(get_data(in_kc2)->keyname[i]);

        if (diff != 0)
        {
            return diff;
        }
    }

    return get_data(in_kc1)->len_name - get_data(in_kc2)->len_name;
}

KeyCell *
key_cell_get_parent (KeyCell *in_kc)
{
    if (key_cell_get_type_id(in_kc) == KEY_ROOT)
    {
        return NULL;
    }
    else
    {
        Cell *hb = hive_get_cell_from_offset (get_hive(in_kc),
                                              get_data(in_kc)->ofs_parent);
        return key_cell_from_cell(hb);
    }
}

SecurityDescriptorCell*
key_cell_get_security_descriptor_cell (KeyCell *in_kc)
{
    return security_descriptor_cell_from_cell(
        hive_get_cell_from_offset (get_hive(in_kc), get_data(in_kc)->ofs_sk));
}

gboolean
key_cell_set_secdesc (KeyCell *in_kc, const SecurityDescriptor *in_secdesc)
{
    SecurityDescriptorCell *sd_cell =
        security_descriptor_cell_alloc(get_hive(in_kc), get_offset(in_kc),
                                       in_secdesc);
    get_data(in_kc)->ofs_sk =
        offset_to_begin(
            cell_get_offset(security_descriptor_cell_to_cell(sd_cell)));

    return TRUE;
}

const SecurityDescriptor*
key_cell_get_secdesc (KeyCell *in_kc)
{
    return security_descriptor_cell_get_secdesc(
        key_cell_get_security_descriptor_cell(in_kc));
}

ValueCell*
key_cell_get_classname_value_cell (KeyCell *in_kc)
{
    if (get_data(in_kc)->len_classname > 0)
    {
        Cell *cell = hive_get_cell_from_offset(get_hive(in_kc),
                                               get_data(in_kc)->ofs_classname);
        return value_cell_from_cell(cell);
    }
    else
    {
        return NULL;
    }
}


/***********************************************************************/

KeyListCell *
key_cell_get_key_list_cell (KeyCell *in_kc)
{
    if (get_data(in_kc)->ofs_lf == -1)
    {
        return NULL;
    }
    else
    {
        return key_list_cell_from_cell(
            hive_get_cell_from_offset (get_hive(in_kc),
                                       get_data(in_kc)->ofs_lf));
    }
}

gboolean
key_cell_set_key_list_cell(KeyCell *in_kc, KeyListCell *in_klc,
                           gboolean in_recursive)
{
    KeyListCell *current = key_cell_get_key_list_cell(in_kc);
    if (current != NULL)
    {
        key_list_cell_unalloc(current, in_recursive);
    }

    if (in_klc != NULL)
    {
        get_data(in_kc)->ofs_lf =
            offset_to_begin(cell_get_offset(key_list_cell_to_cell(in_klc)));
    }
    else
    {
        get_data(in_kc)->ofs_lf = -1;
    }

    return TRUE;
}

guint32
key_cell_get_number_of_subkeys (KeyCell *in_kc)
{
    return get_data(in_kc)->no_subkeys;
}

KeyCell *
key_cell_add_subkey (KeyCell *in_kc, const ustring *name)
{
    KeyCell *result = key_cell_alloc (get_hive(in_kc),
                                      get_offset(in_kc), name);

    if (result == NULL)
    {
        return NULL;
    }

    // The key list cell could be null here. key_list_cell_add_entry
    // is smart enough to add it to the parent if needed.
    gint klc_ret = key_list_cell_add_entry (
        key_cell_get_key_list_cell(in_kc), in_kc, result);
    if (klc_ret == KLC_ADD_ERROR)
    {
        key_cell_unalloc (result, TRUE);
        return NULL;
    }
    else
    {
        if (klc_ret == KLC_ADD_ADDED)
        {
            get_data(in_kc)->no_subkeys++;
        }
        get_data(result)->ofs_parent = offset_to_begin(get_offset(in_kc));
        get_data(result)->ofs_sk = get_data(in_kc)->ofs_sk;
        security_descriptor_cell_increment_use_count(
            key_cell_get_security_descriptor_cell(in_kc));
        key_cell_set_dirty(in_kc);
        return result;
    }
}

static void
key_cell_check_nullify_key_list_cell(KeyCell *in_kc)
{
    if (key_cell_get_number_of_subkeys(in_kc) == 0)
    {
        key_cell_set_key_list_cell(in_kc, NULL, TRUE);
    }
}

gboolean
key_cell_delete_subkey (KeyCell *in_kc, guint32 in_entry)
{
    gboolean ret_val = FALSE;
    KeyListCell *kcl = key_cell_get_key_list_cell(in_kc);
    if (kcl != NULL)
    {
        if (key_list_cell_delete_entry(kcl, in_entry))
        {
            get_data(in_kc)->no_subkeys--;
            key_cell_set_dirty(in_kc);
            ret_val =  TRUE;
        }
    }

    if (ret_val)
    {
        key_cell_check_nullify_key_list_cell(in_kc);
    }

    return ret_val;
}

gboolean
key_cell_delete_subkey_str (KeyCell *in_kc, const char *name)
{
    gboolean ret_val = FALSE;
    KeyListCell *kcl = key_cell_get_key_list_cell(in_kc);
    if (kcl != NULL)
    {
        if (key_list_cell_delete_entry_str(kcl, name))
        {
            get_data(in_kc)->no_subkeys--;
            key_cell_set_dirty(in_kc);
            ret_val = TRUE;
        }
        else
        {
            rra_message(N_("Bad return from key_list_cell_delete_entry_str"));
        }
    }

    if (ret_val)
    {
        key_cell_check_nullify_key_list_cell(in_kc);
    }

    return ret_val;
}

gboolean
key_cell_delete_subkey_ustr (KeyCell *in_kc, const ustring *name)
{
    return key_cell_delete_subkey_str(in_kc, ustr_as_utf8(name));
}

KeyCell *
key_cell_get_subkey (KeyCell *in_kc, guint32 in_entry)
{
    KeyListCell *kcl = key_cell_get_key_list_cell(in_kc);
    if (kcl == NULL)
    {
        return NULL;
    }
    else
    {
        return key_list_cell_get_entry(kcl, in_entry);
    }
}

KeyCell *
key_cell_get_subkey_str (KeyCell *in_kc, const char *in_name)
{
    KeyListCell *kcl = key_cell_get_key_list_cell(in_kc);
    if (kcl == NULL)
    {
        return NULL;
    }
    else
    {
        return key_list_cell_get_entry_str (kcl, in_name);
    }
}

KeyCell *
key_cell_get_subkey_ustr (KeyCell *in_kc, const ustring *in_name)
{
    return key_cell_get_subkey_str(in_kc, ustr_as_utf8(in_name));
}


/************************************************************************/

ValueListCell *
key_cell_get_value_list_cell (KeyCell *in_kc)
{
    return value_list_cell_from_cell(
        hive_get_cell_from_offset (get_hive(in_kc),
                                   get_data(in_kc)->ofs_vallist));
}

guint32
key_cell_get_number_of_values(KeyCell *kc)
{
    return get_data(kc)->no_values;
}

ValueKeyCell *
key_cell_get_value (KeyCell *in_kc, guint32 in_off)
{
    ValueListCell *vlist = key_cell_get_value_list_cell (in_kc);
    return value_list_cell_get_entry(vlist, in_off,
                                     (int)get_data(in_kc)->no_values);
}

ValueKeyCell *
key_cell_get_value_str (KeyCell *in_kc, const char *in_name)
{
    gint32 index = key_cell_find_value(in_kc, in_name);
    if (index == -1)
    {
        return NULL;
    }
    else
    {
        return key_cell_get_value(in_kc, index);
    }
}

ValueKeyCell *
key_cell_get_value_ustr (KeyCell *in_kc, const ustring *in_name)
{
    return key_cell_get_value_str(in_kc, ustr_as_utf8(in_name));
}

// return offset of value of name 'name' (or -1)
static gint32
key_cell_find_value (KeyCell *in_kc, const char *in_name)
{
    gint32 ret_val = -1;
    guint32 i;
    ustring *name_ustr = ustr_create(in_name);

    for (i = 0; i < get_data(in_kc)->no_values; i++)
    {
        ustring *vkc_name =
            value_key_cell_get_name (key_cell_get_value (in_kc, i));
        if (0 == ustr_regcmp (name_ustr, vkc_name))
        {
            ret_val = i;
            ustr_free(vkc_name);
            break;
        }

        ustr_free(vkc_name);
    }

    ustr_free(name_ustr);

    return ret_val;
}

gboolean
key_cell_delete_value (KeyCell *in_kc, guint32 in_index)
{
    if (in_index >= (int)get_data(in_kc)->no_values)
    {
        rra_message(N_("value number outside of range."));
        return FALSE;
    }

    ValueKeyCell *vk = key_cell_get_value (in_kc, in_index);
    ValueListCell *oldblk = key_cell_get_value_list_cell(in_kc);

    value_list_cell_remove(oldblk, in_index, get_data(in_kc)->no_values);
    get_data(in_kc)->no_values--;

    gboolean ret_val = value_key_cell_unalloc (vk);
    if (ret_val)
    {
        key_cell_set_dirty(in_kc);
    }
    return ret_val;
}

gboolean
key_cell_delete_value_str (KeyCell *in_kc, const char *in_name)
{
    gint32 index = key_cell_find_value(in_kc, in_name);
    if (index == -1)
    {
        return FALSE;
    }
    else
    {
        return key_cell_delete_value(in_kc, index);
    }
}

gboolean
key_cell_delete_value_ustr (KeyCell *in_kc, const ustring *in_name)
{
    return key_cell_delete_value_str(in_kc, ustr_as_utf8(in_name));
}

ValueKeyCell*
key_cell_add_value (KeyCell *in_kc, const char *in_name, const Value *in_val)
{
    Cell *oldvlist = NULL;
    ValueListCell *newvlist = NULL;
    ValueKeyCell *newvkkey = NULL;

    if(in_name == NULL)
    {
        rra_message(N_("got null name"));
        return NULL;
    }

    //TODO: check to see if the value exists already or not

    if (get_data(in_kc)->no_values)
    {
        oldvlist = hive_get_cell_from_offset (get_hive(in_kc),
                                              get_data(in_kc)->ofs_vallist);
    }

    newvlist = value_list_cell_alloc (get_hive(in_kc), get_offset(in_kc),
                                      get_data(in_kc)->no_values + 1);
    if (newvlist == NULL)
    {
        return NULL;
    }

    if (oldvlist)
    {   /* Copy old data if any */
        memcpy (cell_get_data(value_list_cell_to_cell(newvlist)) + 4,
                cell_get_data(oldvlist) + 4,
                get_data(in_kc)->no_values * 4 + 4);
    }

    newvkkey = value_key_cell_alloc (
        get_hive(in_kc), cell_get_offset(value_list_cell_to_cell(newvlist)),
        in_name, in_val);

    if (newvkkey == NULL)
    {
        value_list_cell_unalloc(newvlist, FALSE,
                                key_cell_get_number_of_values(in_kc));
        return NULL;
    }

    /* Add pointer in value list */
    value_list_cell_add(
        newvlist,
        cell_get_offset(value_key_cell_to_cell(newvkkey)),
        get_data(in_kc)->no_values);

    /* Finally update the key and free the old valuelist */
    get_data(in_kc)->no_values++;
    get_data(in_kc)->ofs_vallist =
        offset_to_begin(cell_get_offset(value_list_cell_to_cell(newvlist)));
    if (oldvlist)
    {
        cell_unalloc (oldvlist);
    }
    key_cell_set_dirty(in_kc);
    return newvkkey;
}

/**********************************************************************/

void
key_cell_pretty_print (KeyCell *in_key, const char *in_pathname)
{
    ustring *out = ustr_new();

    if (key_cell_get_pretty_output(in_key, in_pathname, out))
    {
        printf("%s", ustr_as_utf8(out));
    }

    ustr_free(out);
}

void
key_cell_debug_print (KeyCell *in_key)
{
    ustring *out = ustr_new();

    if (key_cell_get_xml_output(in_key, out))
    {
        fprintf(stderr, "%s", ustr_as_utf8(out));
    }

    ustr_free(out);
}

gboolean
key_cell_get_pretty_output (KeyCell *in_key, const char *in_pathname,
                            ustring *in_output)
{
    if (in_key == NULL || in_output == NULL || in_pathname == NULL)
    {
        return FALSE;
    }

    ustr_printfa (in_output, _("Key %s:\n"), in_pathname);

    ustring *name = key_cell_get_name(in_key);
    ustr_printfa (in_output, _("  Name %s:\n"), ustr_as_utf8(name));
    ustr_free(name);

    ustring *classname = key_cell_get_classname(in_key);
    if (ustr_strlen(classname) > 0)
    {
        ustr_printfa (in_output, _("  Classname: %s\n"),
                      ustr_as_utf8(classname));
    }
    ustr_free(classname);

    if (key_cell_get_number_of_values(in_key) > 0)
    {
        guint i;
        ustr_printfa (in_output, _("  Values:\n"));

        for (i = 0; i < key_cell_get_number_of_values(in_key); i++)
        {
            ValueKeyCell *vkc = key_cell_get_value(in_key, i);
            ustring *vkc_name = value_key_cell_get_name(vkc);
            Value *val = value_key_cell_get_val(vkc);
            ustring *val_value = value_get_as_string(val);
            const char *type_str = value_type_get_string(
                value_key_cell_get_type(vkc));

            if (val_value == NULL)
            {
                ustr_printfa (in_output,
                              _("    \"%s\" (%s)=> <No Value Generated>\n"),
                              ustr_as_utf8(vkc_name), type_str);
            }
            else if (ustr_strlen(val_value) > 50
                     || value_key_cell_get_type(vkc) == REG_EXPAND_SZ)
            {
                ustr_printfa (in_output,
                              _("    \"%s\" (%s)=> <Not Displayed>\n"),
                              ustr_as_utf8(vkc_name), type_str);
            }
            else
            {
                ustr_printfa (in_output, _("    \"%s\" (%s)=> %s\n"),
                              ustr_as_utf8(vkc_name),
                              type_str,
                              ustr_as_utf8(val_value));
            }

            ustr_free(val_value);
            value_free(val);
            ustr_free(vkc_name);
        }
    }

    if (key_cell_get_number_of_subkeys(in_key) > 0)
    {
        guint i;
        ustr_printfa (in_output, _("  Subkeys:\n"));

        for (i = 0; i < key_cell_get_number_of_subkeys(in_key); i++)
        {
            KeyCell *subk = key_cell_get_subkey(in_key, i);
            ustring *subk_name = key_cell_get_name(subk);
            ustr_printfa (in_output, _("    %s\n"), ustr_as_utf8(subk_name));
            ustr_free(subk_name);
        }
    }
    return TRUE;
}

static void
output_name_to_ustring(const char *in_val, ustring *in_output)
{
    gchar *escaped_str = g_strescape(in_val, NULL);
    ustr_printfa(in_output, _("\"%s\"), "), escaped_str);
    g_free(escaped_str);
}

gboolean
key_cell_get_parseable_output (KeyCell *in_key, const char *in_pathname,
                               ustring *in_output)
{
    if (in_key == NULL || in_output == NULL || in_pathname == NULL)
    {
        return FALSE;
    }

    ustr_printfa (in_output, _("Key %s:\n"), in_pathname);

    ustring *name = key_cell_get_name(in_key);
    ustr_printfa (in_output, _("  Name %s:\n"), ustr_as_utf8(name));
    ustr_free(name);

    ustring *classname = key_cell_get_classname(in_key);
    if (ustr_strlen(classname) > 0)
    {
        ustr_printfa (in_output, _("  Classname: %s\n"),
                      ustr_as_utf8(classname));
    }
    ustr_free(classname);

    if (key_cell_get_number_of_values(in_key) > 0)
    {
        guint i;
        ustr_printfa (in_output, _("  Values: [ "));

        for (i = 0; i < key_cell_get_number_of_values(in_key); i++)
        {
            ValueKeyCell *vkc = key_cell_get_value(in_key, i);
            ustring *vkc_name = value_key_cell_get_name(vkc);
            output_name_to_ustring(ustr_as_utf8(vkc_name), in_output);
            ustr_free(vkc_name);
        }
        ustr_printfa (in_output, _("  ]\n"));
    }

    if (key_cell_get_number_of_subkeys(in_key) > 0)
    {
        guint i;
        ustr_printfa (in_output, _("  Subkeys: [ "));

        for (i = 0; i < key_cell_get_number_of_subkeys(in_key); i++)
        {
            KeyCell *subk = key_cell_get_subkey(in_key, i);
            ustring *subk_name = key_cell_get_name(subk);
            output_name_to_ustring(ustr_as_utf8(subk_name), in_output);
            ustr_free(subk_name);
        }
        ustr_printfa (in_output, _("  ]\n"));
    }
    return TRUE;
}

gboolean
key_cell_get_xml_output (KeyCell *in_key, ustring *in_output)
{
    if (in_key == NULL || in_output == NULL)
    {
        return FALSE;
    }

    ustr_printfa (in_output, "<KeyCell offset=\"%#010x\" size=\"%d\">\n",
                  offset_to_begin(get_offset(in_key)),
                  get_data(in_key)->cell_head.size);
    ustr_printfa (in_output, "  <id value=\"%#06x\" value_str=\"%s\"/>\n",
                  get_data(in_key)->cell_head.id,
                  cell_get_id_str(key_cell_to_cell(in_key)));
    ustr_printfa (in_output, "  <type value=\"%#06x\" value_str=\"%s\"/>\n",
                  get_data(in_key)->type, key_cell_get_type_str(in_key));
    ustr_printfa (in_output, "  <timestamp value=\"%#" G_GINT64_MODIFIER
                  "x\"/>\n",
                  get_data(in_key)->timestamp);
    ustr_printfa (in_output, "  <unknown1 value=\"%#010x\"/>\n",
                  get_data(in_key)->unknown1);
    ustr_printfa (in_output, "  <parent_key_offset value=\"%#010x\"/>\n",
                  get_data(in_key)->ofs_parent);
    ustr_printfa (in_output, "  <no_subkeys value=\"%u\"/>\n",
                  get_data(in_key)->no_subkeys);
    ustr_printfa (in_output, "  <unknown2 value=\"%#010x\"/>\n",
                  get_data(in_key)->unknown2);
    ustr_printfa (in_output, "  <lf_record_offset value=\"%#010x\"/>\n",
                  get_data(in_key)->ofs_lf);
    ustr_printfa (in_output, "  <ofs_unknown1 value=\"%#010x\"/>\n",
                  get_data(in_key)->ofs_unknown1);
    ustr_printfa (in_output, "  <number_of_values value=\"%u\"/>\n",
                  get_data(in_key)->no_values);
    ustr_printfa (in_output, "  <val_list_offset value=\"%#010x\"/>\n",
                  get_data(in_key)->ofs_vallist);
    ustr_printfa (in_output, "  <sk_record_offset value=\"%#010x\"/>\n",
                  get_data(in_key)->ofs_sk);
    ustr_printfa (in_output, "  <classname_offset value=\"%#010x\"/>\n",
                  get_data(in_key)->ofs_classname);
    ustr_printfa (in_output,
                  "  <unknown3 value=\"%#010x,%#010x,%#010x,%#010x\"/>\n",
                  get_data(in_key)->unknown3[0],
                  get_data(in_key)->unknown3[1],
                  get_data(in_key)->unknown3[2],
                  get_data(in_key)->unknown3[3]);
    ustr_printfa (in_output, "  <unknown4 value=\"%#010x\"/>\n",
                  get_data(in_key)->unknown4);
    ustr_printfa (in_output, "  <name_length value=\"%u\"/>\n",
                  get_data(in_key)->len_name);
    ustr_printfa (in_output, "  <classname_length value=\"%u\"/>\n",
                  get_data(in_key)->len_classname);

    ustr_printfa (in_output, "  <name>");
    ustring *name = key_cell_get_name(in_key);
    ustr_printfa (in_output, "%s", ustr_as_utf8(name));
    ustr_free(name);
    ustr_printfa (in_output, "</name>\n");

    ustr_printfa (in_output, "</KeyCell>\n");
    return TRUE;
}

static gboolean
walk_tree_checker(KeyCell *in_kc, RRACheckData *in_data)
{
    if (in_kc == NULL)
    {
        rra_check_error(in_data, _("NULL key cell"));
        return FALSE;
    }

    if (!key_cell_is_valid(in_kc))
    {
        rra_check_error(in_data, _("Invalid key cell"));
        return FALSE;
    }
    gboolean ret_val = TRUE;

    rra_check_checking(in_data, _("for a valid type"));
    /* type ok */
    if (get_data(in_kc)->type != KEY_ROOT
        && get_data(in_kc)->type != KEY_NORMAL
        && get_data(in_kc)->type != KEY_LINK)
    {
        rra_check_error(in_data, _("type"));
        ret_val = FALSE;
    }

    /* This test is broken.  We are finding keys marked as root down
       somewhere in the tree.  It must mean something other than root.
    */
#if 0
    rra_check_checking(in_data, _("for no parent on root key"));
    /* this seems like it'd be true, but in practice nope */
    if (get_data(in_kc)->type == KEY_ROOT
        && get_data(in_kc)->ofs_parent != 0)
    {
        rra_check_info(in_data, _("root key has non 0 parent offset"));
        /* Seems this happens.  How ugly. */
        /* ret_val = FALSE; */
    }
#endif

    rra_check_checking(in_data, _("for valid name size"));
    /* name size ok. Max size is 255 (*2 for utf16) */
    if (get_data(in_kc)->len_name == 0
        || get_data(in_kc)->len_name > 255 * 2)
    {
        rra_check_error(in_data, _("name length"));
        ret_val = FALSE;
    }

    rra_check_checking(in_data, _("for valid cell size"));
    /* check size equalities */
    if ((sizeof(struct KeyCellData) + get_data(in_kc)->len_name)
        > cell_size(key_cell_to_cell(in_kc)))
    {
        rra_check_error(
            in_data,
            _("Mismatch Cell / KeyCell length"));
        ret_val = FALSE;
    }

    rra_check_checking(in_data, _("for subkey offset with no subkeys"));
    /* no subkeys. shouldn't be an offset to a KeyListCell */
    if ((get_data(in_kc)->no_subkeys == 0 && get_data(in_kc)->ofs_lf != -1)
        || (get_data(in_kc)->ofs_lf == -1 && get_data(in_kc)->no_subkeys != 0))
    {
        rra_check_error(in_data,
                        _(INVALID_KEYCELL_MSG
                          "Subkey count / Subkey list mismatch"));
        ret_val =  FALSE;
    }

    rra_check_checking(in_data,
                       _("for classname offset and zero length classname"));
    /* No classname shouldn't be offset */
    if ((get_data(in_kc)->len_classname == 0
         && get_data(in_kc)->ofs_classname != -1)
        || (get_data(in_kc)->ofs_classname == -1
            && get_data(in_kc)->len_classname != 0))
    {
        rra_check_error(in_data,
                        _(INVALID_KEYCELL_MSG
                          "Classname / Classname offset mismatch"));
        ret_val = FALSE;
    }

    rra_check_checking(in_data, _("for zero values and value offset"));
    /* Values count and offset
     * Looks like this can happen.  Though I bet it shouldn't */
    if ((get_data(in_kc)->no_values == 0
         && get_data(in_kc)->ofs_vallist != -1)
        || (get_data(in_kc)->ofs_vallist == -1
            && get_data(in_kc)->no_values != 0))
    {
        rra_check_warning(
            in_data,
            _("no_values (%d) and ofs_vallist (%#010x) don't match"),
            get_data(in_kc)->no_values, get_data(in_kc)->ofs_vallist);
        /* Seems this happens.  How ugly. */
        /* ret_val = FALSE; */
    }

    rra_check_checking(in_data, _("for a valid value list key offset"));
    if (get_data(in_kc)->ofs_vallist != -1
        && !hive_is_valid_offset(get_hive(in_kc), get_data(in_kc)->ofs_vallist))
    {
        rra_check_error(in_data,
                        _("invalid vallist offset"));
        ret_val = FALSE;
    }

    rra_check_checking(in_data, _("for a valid classname offset"));
    if (get_data(in_kc)->ofs_classname != -1
        && !hive_is_valid_offset(get_hive(in_kc), get_data(in_kc)->ofs_classname))
    {
        rra_check_error(in_data,
                        _("invalid classname offset"));
        ret_val = FALSE;
    }

    rra_check_checking(in_data, _("for a valid key list cell offset"));
    if (get_data(in_kc)->ofs_lf != -1
        && !hive_is_valid_offset(get_hive(in_kc), get_data(in_kc)->ofs_lf))
    {
        rra_check_error(in_data,
                        _("invalid subkey list offset"));
        ret_val = FALSE;
    }

    rra_check_checking(in_data, _("for a valid parent offset"));
    if (get_data(in_kc)->ofs_parent != 0 /* WinXP */
        && get_data(in_kc)->ofs_parent != -1 /* Win2003server */
        && !hive_is_valid_offset(get_hive(in_kc), get_data(in_kc)->ofs_parent))
    {
        rra_check_error(in_data,
                        _("invalid parent offset"));
        ret_val = FALSE;
    }

    rra_check_checking(in_data, _("valid security descriptor cell offset"));
    if (!hive_is_valid_offset(get_hive(in_kc), get_data(in_kc)->ofs_sk))
    {
        rra_check_error(in_data,
                        _("valid sk offset"));
        ret_val = FALSE;
    }

    rra_check_checking(in_data,
                       _("correct key ordering"));
    if (key_cell_get_number_of_subkeys(in_kc) >= 2)
    {
        int i = 0;
        KeyCell *ksub1 = key_cell_get_subkey(in_kc, i);
        i++;
        KeyCell *ksub2 = key_cell_get_subkey(in_kc, i);
        i++;
        do {
            if (key_cell_compare_names(ksub1, ksub2) >= 0)
            {
                rra_check_error(in_data,
                                _("Key cells not in good order"));
                ret_val = FALSE;
            }

            ksub1 = ksub2;
            ksub2 = key_cell_get_subkey(in_kc, i);
            i++;
        } while(i < key_cell_get_number_of_subkeys(in_kc));
    }

    /* This is in fact more the rule than the exception.  Bah. */
    if (FALSE)
    {
        rra_check_checking(in_data,
                           _("correct value ordering"));
        if (key_cell_get_number_of_values(in_kc) >= 2)
        {
            int i = 0;
            ValueKeyCell *vsub1 = key_cell_get_value(in_kc, i);
            i++;
            ValueKeyCell *vsub2 = key_cell_get_value(in_kc, i);
            i++;
            do {
                ustring *from_name = value_key_cell_get_name(vsub1);
                ustring *to_name = value_key_cell_get_name(vsub2);
                int val_comp = ustr_compare(from_name, to_name);
                ustr_free(from_name);
                ustr_free(to_name);

                if (val_comp >= 0)
                {
                    rra_check_error(
                        in_data,
                        _("ValueKey cells not in alphabetical order"));
                    ret_val = FALSE;
                }

                vsub1 = vsub2;
                vsub2 = key_cell_get_value(in_kc, i);
                i++;
            } while (i < key_cell_get_number_of_values(in_kc));
        }
    }

    rra_check_checking(in_data,
                       _("that no subkeys or values have same name"));
    if (key_cell_get_number_of_subkeys(in_kc) > 0
        && key_cell_get_number_of_values(in_kc) > 0)
    {
        int kindex = 0;
        KeyCell *ksub = key_cell_get_subkey(in_kc, kindex);
        ustring *kname = key_cell_get_name(ksub);

        int vindex = 0;
        ValueKeyCell *vsub = key_cell_get_value(in_kc, vindex);
        ustring *vname = value_key_cell_get_name(vsub);

        while(TRUE)
        {
            gboolean inc_both = FALSE;
            int cmp_val = ustr_compare(kname, vname);
            if (cmp_val == 0)
            {
                rra_check_warning(in_data,
                                  _("sub key and value with same name: %s"),
                                  ustr_as_utf8(kname));
                /* Seems this happens.  How ugly. */
                /* ret_val = FALSE; */
                if (key_cell_get_value_str(ksub, "") != NULL)
                {
                    rra_check_warning(in_data,
                                      _("value and sub key \"\" value: %s"),
                                      ustr_as_utf8(kname));
                }

                inc_both = TRUE;
            }

            if (cmp_val < 0 || inc_both)
            {
                kindex++;
                if (kindex >= key_cell_get_number_of_subkeys(in_kc))
                {
                    break;
                }
                ustr_free(kname);
                ksub = key_cell_get_subkey(in_kc, kindex);
                kname = key_cell_get_name(ksub);
            }

            if (cmp_val > 0 || inc_both)
            {
                vindex++;
                if (vindex >= key_cell_get_number_of_values(in_kc))
                {
                    break;
                }
                ustr_free(vname);
                vsub = key_cell_get_value(in_kc, vindex);
                vname = value_key_cell_get_name(vsub);
            }
        }

        ustr_free(kname);
        ustr_free(vname);
    }

    if (!ret_val)
    {
        key_cell_pretty_print(in_kc, "");
    }

    /* walk into subkeys */
    guint i;
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
hcheck_key_cell_pass(Hive *in_hive, RRACheckData *in_data)
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
