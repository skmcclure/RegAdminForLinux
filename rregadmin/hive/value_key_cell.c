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

#include <errno.h>
#include <string.h>
#include <ctype.h>

#include <glib/gi18n-lib.h>

#include <rregadmin/hive/value_key_cell.h>

#include <rregadmin/util/macros.h>
#include <rregadmin/hive/log.h>
#include <rregadmin/util/giochan_helpers.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/util/value_type.h>
#include <rregadmin/hive/bin.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/util/check.h>
#include <rregadmin/hive/hcheck_decl.h>
#include <rregadmin/hive/defines.h>
#include <rregadmin/hive/value_cell.h>
#include <rregadmin/hive/key_cell.h>

/** This is the value key cell
 *
 * If the sign bit (31st bit) in the length field is set, the value is
 * stored inline this struct, and not in a seperate data chunk -
 * the data then seems to be in the type field, and maybe also
 * in the flag and dummy1 field if -len > 4 bytes
 * If the name size == 0, then the struct is probably cut int16_t right
 * after the val_type or flag.
 * The flag meaning is rather unknown.
 *
 * NOTE: len_data
 *
 * The values for len_data are as follows:
 *
 * len_data == 0x00000000       data length is 0
 * len_data == 0x80000000       data are stored in val_type field,
 *                              TYPE == REG_DWORD (I think)
 * len_data & 0x80000000        data are stored in ofs_data field
 *
 * @internal
 *
 * @ingroup vkc_hive_group
 */
struct ValueKeyCellData
{
    /** Magic number
     *
     * Is always "vk" = 0x6B76
     */
    struct CellHeader cell_head;
    /** Length of the name.
     */
    int16_t len_name;
    /** Length of the data.
     */
    guint32 len_data;
    /** Offset to the ValueCell
     */
    gint32 ofs_data;
    /** Type of the value
     */
    gint32 val_type;
    /** Name type flag
     *
     * 0:name is utf16, 1:name is ascii (I think)
     */
    guint16 flag;
    /** Not sure what this is.
     */
    guint16 dummy1;
    /** Variable length array holding the name.
     */
    guint8 keyname[];
} RRA_VERBATIM_STRUCT;

/** Object wrapper for value key cells.
 *
 * @ingroup vkc_hive_group
 */
struct ValueKeyCell_
{
    Cell cell;
};

static gboolean is_inline_data_length(const ValueKeyCell *p);

static struct ValueKeyCellData* get_data(ValueKeyCell *kc);
static Hive* get_hive(const ValueKeyCell *kc);
static offset get_offset(ValueKeyCell *kc);

static inline struct ValueKeyCellData*
get_data(ValueKeyCell *kc)
{
    return (struct ValueKeyCellData*)cell_get_data(value_key_cell_to_cell(kc));
}

static struct ValueKeyCellData*
get_data_const(const ValueKeyCell *kc)
{
    return (struct ValueKeyCellData*)cell_get_data(
        value_key_cell_to_cell((ValueKeyCell*)kc));
}

static Hive*
get_hive(const ValueKeyCell *kc)
{
    return cell_get_hive(value_key_cell_to_cell((ValueKeyCell*)kc));
}

static offset
get_offset(ValueKeyCell *kc)
{
    return cell_get_offset(value_key_cell_to_cell(kc));
}

ValueKeyCell *
value_key_cell_alloc (Hive *hdesc, offset rel, const char *name,
                      const Value *in_val)
{
    Cell *new_cell;
    ValueKeyCell *vk;

    new_cell = cell_alloc (hdesc, rel,
                           sizeof (struct ValueKeyCellData) + strlen (name));

    /* Allocate vk descriptor including its name */
    if (new_cell == NULL)
    {
        return NULL;
    }
    vk = (ValueKeyCell *) new_cell;

    /* Fill in vk struct */
    get_data(vk)->cell_head.id = ID_VK_KEY;
    get_data(vk)->len_name = strlen (name);

    /* I think this is 1 -> name is ascii, 0 -> name is utf16 */
    get_data(vk)->flag = 1;
    get_data(vk)->dummy1 = 0;
    strcpy ((char *) &get_data(vk)->keyname, name);

    get_data(vk)->ofs_data = 0;

    if (!value_key_cell_set_val(vk, in_val))
    {
        cell_unalloc(new_cell);
        return NULL;
    }

    return vk;
}

gboolean
value_key_cell_unalloc (ValueKeyCell *vk)
{
    rra_info(N_("Unallocating (%#010x)"),
             offset_to_begin(cell_get_offset(value_key_cell_to_cell(vk))));
    if (!is_inline_data_length(vk))
    {
        value_cell_unalloc (value_key_cell_get_value_cell(vk));
    }

    return cell_unalloc (value_key_cell_to_cell(vk));
}

ValueKeyCell *
value_key_cell_from_cell (Cell *p)
{
    ValueKeyCell *result = (ValueKeyCell*)p;

    if (get_data(result)->cell_head.id != ID_VK_KEY)
    {
        rra_message(
            N_("Unexpected id in cell (%p) supposed to be value: %#06x"),
            p, get_data(result)->cell_head.id);
        return NULL;
    }

    return result;
}

Cell*
value_key_cell_to_cell(ValueKeyCell *vkc)
{
    return (Cell*)vkc;
}

static gboolean
is_inline_data_length(const ValueKeyCell *p)
{
    int ret_val = (get_data_const(p)->len_data >= 0x8000000
                   && get_data_const(p)->len_data <= 0x80000004);
    return ret_val ? TRUE : FALSE;
}

gboolean
value_key_cell_is_valid(const ValueKeyCell *p_const)
{
    ValueKeyCell *p = (ValueKeyCell*)p_const;

    if (p == NULL)
    {
        rra_message(N_("ValueKeyCell is NULL"));
        return FALSE;
    }

    // id
    if (get_data(p)->cell_head.id != ID_VK_KEY)
    {
        rra_message(N_("Invalid id %d"), get_data(p)->cell_head.id);
        return FALSE;
    }

    return TRUE;
}

ValueCell *
value_key_cell_get_value_cell(const ValueKeyCell *vk)
{
    if (get_data_const(vk)->ofs_data != 0
        && !is_inline_data_length(vk))
    {
        return value_cell_from_cell(
            hive_get_cell_from_offset(get_hive(vk),
                                      get_data_const(vk)->ofs_data));
    }
    else
    {
        return NULL;
    }
}

gboolean
value_key_cell_set_val (ValueKeyCell *in_vkc, const Value *in_val)
{
    gboolean ret_val = TRUE;
    Hive *hive = cell_get_hive(value_key_cell_to_cell(in_vkc));

    get_data(in_vkc)->val_type = value_get_type(in_val);

    GByteArray *val_data = value_encode(in_val, VALUE_STR_UTF16);

    if (value_get_type(in_val) == REG_DWORD
        || value_get_type(in_val) == REG_DWORD_BIG_ENDIAN)
    {
        ValueCell *vc = value_key_cell_get_value_cell(in_vkc);
        if (vc != NULL)
        {
            value_cell_unalloc(vc);
        }
        get_data(in_vkc)->len_data = 0x80000004;
        memcpy (&get_data(in_vkc)->ofs_data, val_data->data, 4);
    }
    else
    {
        ValueCell *vc = value_key_cell_get_value_cell(in_vkc);
        offset null_ofs = INVALID_OFFSET;

        if (vc == NULL)
        {
            vc = value_cell_alloc(hive, null_ofs,
                                  val_data->data, val_data->len);
        }
        else if (value_cell_get_data_length(vc) < val_data->len)
        {
            value_cell_unalloc(vc);
            vc = value_cell_alloc(hive, null_ofs,
                                  val_data->data, val_data->len);
        }
        else
        {
            value_cell_set (vc, val_data->data, val_data->len);
        }

        if (vc == NULL)
        {
            ret_val = FALSE;
        }
        else
        {
            get_data(in_vkc)->ofs_data =
                offset_to_begin(cell_get_offset(value_cell_to_cell(vc)));
            get_data(in_vkc)->len_data = val_data->len;
        }
    }

    g_byte_array_free(val_data, TRUE);

    if (ret_val)
    {
        bin_set_dirty(cell_get_bin(value_key_cell_to_cell(in_vkc)));
    }

    return ret_val;
}

gboolean
value_key_cell_modify_val (ValueKeyCell *in_vkc,
                           ValueKeyCellModifier in_mod,
                           gpointer in_mod_data)
{
    if (in_vkc == NULL || in_mod == NULL)
    {
        return FALSE;
    }

    Value *da_val = value_key_cell_get_val(in_vkc);
    da_val = in_mod(da_val, in_mod_data);

    if (da_val == NULL)
    {
        return FALSE;
    }

    gboolean ret_val = value_key_cell_set_val(in_vkc, da_val);
    value_free(da_val);

    return ret_val;
}

Value *
value_key_cell_get_val (ValueKeyCell *vk)
{
    ValueType da_type = value_key_cell_get_type(vk);

    if (da_type == REG_NONE)
    {
        return value_create_none();
    }
    else if (da_type == REG_DWORD)
    {
        return value_create_dword(*(dword_type*)value_key_cell_get_data(vk));
    }
    else if (da_type == REG_DWORD_BIG_ENDIAN)
    {
        return value_create_dword_be(
            *(dword_type*)value_key_cell_get_data(vk));
    }
    else if (da_type == REG_BINARY)
    {
        const guint8* data = value_key_cell_get_data(vk);
        if (data == NULL)
        {
            return NULL;
        }
        else
        {
            return value_create_binary(data, value_key_cell_get_data_size(vk));
        }
    }
    else if (da_type == REG_SZ)
    {
        const guint8* data = value_key_cell_get_data(vk);
        if (data == NULL)
        {
            return NULL;
        }
        else
        {
            /*
             * If the length is odd that's just bad since the encoding
             * is 2 bytes/char.  So correct for that idiocy.
             */
            int len = value_key_cell_get_data_size(vk);
            if (len % 2 != 0)
            {
                rra_message(N_("Dropping data len by 1 for SZ"));
                len--;
            }

            return value_create_string((char*)data, len, USTR_TYPE_UTF16LE);
        }
    }
    else if (da_type == REG_EXPAND_SZ)
    {
        const guint8* data = value_key_cell_get_data(vk);
        if (data == NULL)
        {
            return NULL;
        }
        else
        {
            /*
             * If the length is odd that's just bad since the encoding
             * is 2 bytes/char.  So correct for that idiocy.
             */
            int len = value_key_cell_get_data_size(vk);
            if (len % 2 != 0)
            {
                rra_message(N_("Dropping data len by 1 for EXPAND_SZ"));
                len--;
            }

            return value_create_expanded_string(
                (char*)data, len, USTR_TYPE_UTF16LE);
        }
    }
    else if (da_type == REG_MULTI_SZ)
    {
        const guint8* data = value_key_cell_get_data(vk);
        if (data == NULL)
        {
            return NULL;
        }
        else
        {
            /*
             * If the length is odd that's just bad since the encoding
             * is 2 bytes/char.  So correct for that idiocy.
             */
            int len = value_key_cell_get_data_size(vk);
            if (len % 2 != 0)
            {
                rra_message(N_("Dropping data len by 1 for MULTI_SZ"));
                len--;
            }

            return value_create_multi_string_bin(
                (char*)data, len,
                USTR_TYPE_UTF16LE);
        }
    }
    else if (da_type == REG_RESOURCE_LIST
             || da_type == REG_FULL_RESOURCE_DESCRIPTOR
             || da_type == REG_RESOURCE_REQUIREMENTS_LIST
             || da_type == REG_LINK)
    {
        const guint8* data = value_key_cell_get_data(vk);
        if (data == NULL)
        {
            return NULL;
        }
        else
        {
            return value_create_from_binary(
                data, value_key_cell_get_data_size(vk),
                da_type);
        }
    }
    else if (da_type == REG_QWORD)
    {
        ValueCell *vc = value_key_cell_get_value_cell(vk);
        if (vc == NULL)
        {
            return NULL;
        }
        return value_create_qword(*((qword_type*)value_cell_get_data(vc)));
    }
    else
    {
        rra_message(N_("Invalid value type: %d.  Returning a none value."),
                    da_type);
        return value_create_none();
    }
}

// Pull data from a key
ValueType
value_key_cell_get_type (const ValueKeyCell * vk)
{
    if (get_data_const(vk)->len_data == 0x80000000)
    {
        return REG_DWORD;
    }

    gint32 type = get_data_const(vk)->val_type;
    if (value_type_is_valid(type))
    {
        return type;
    }
    else
    {
        return REG_NONE;
    }
}

guint32
value_key_cell_get_data_size (const ValueKeyCell *vk)
{
    if (get_data_const(vk)->len_data & 0x80000000)
    {
        return 4;
    }
    ValueCell *vc = value_key_cell_get_value_cell(vk);
    guint32 vc_len = value_cell_get_data_length(vc);
    guint32 vkc_len = (get_data_const(vk)->len_data & 0x7fffffff);

    if (vc_len < vkc_len)
    {
        rra_message(
            N_("ValueCell length less than ValueKeyCell length: %d vs %d"),
            vc_len, vkc_len);
        return vc_len;
    }
    else
    {
        return vkc_len;
    }
}

const guint8*
value_key_cell_get_data (const ValueKeyCell *vk)
{
    ValueType da_type = value_key_cell_get_type(vk);

    if (da_type == REG_DWORD)
    {
        if (get_data_const(vk)->len_data == 0x80000000)
        {
            return (guint8*)&get_data_const(vk)->val_type;
        }
        else
        {
            return (guint8*)&get_data_const(vk)->ofs_data;
        }
    }
    else if (da_type == REG_DWORD_BIG_ENDIAN)
    {
        return (guint8*)&get_data_const(vk)->ofs_data;
    }
    else
    {
        if (is_inline_data_length(vk))
        {
            return (guint8*)&get_data_const(vk)->ofs_data;
        }
        else
        {
            ValueCell *vc = value_key_cell_get_value_cell(vk);
            if (vc == NULL)
            {
                return NULL;
            }

            return value_cell_get_data(vc);
        }
    }
}

guint16
value_key_cell_get_flags (const ValueKeyCell *vk)
{
    return get_data_const(vk)->flag;
}

ustring*
value_key_cell_get_name (const ValueKeyCell *vk)
{
    ustring *ret_val = ustr_new();

    if (get_data_const((ValueKeyCell*)vk)->len_name == 0)
    {
        ustr_strset(ret_val, "");
    }
    else
    {
        switch (get_data_const((ValueKeyCell*)vk)->flag)
        {
        case 0:
            ustr_strnset_type(ret_val, USTR_TYPE_UTF16LE,
                              (char*)get_data_const(vk)->keyname,
                              get_data_const(vk)->len_name);
            break;
        case 1:
            ustr_strnset_type(ret_val, USTR_TYPE_ISO8859_1,
                              (char*)get_data_const(vk)->keyname,
                              get_data_const(vk)->len_name);
            break;
        default:
            rra_message(N_("I don't know how to deal with type %d"),
                        get_data_const(vk)->flag);
            ustr_free(ret_val);
            return NULL;
        }
    }

    return ret_val;
}

void
value_key_cell_debug_print (ValueKeyCell *in_vkc)
{
    ustring *out = ustr_new();

    if (value_key_cell_get_xml_output(in_vkc, out))
    {
        fprintf(stderr, "%s", ustr_as_utf8(out));
    }

    ustr_free(out);
}

void
value_key_cell_pretty_print (ValueKeyCell *in_vkc, const char *in_pathname)
{
    ustring *out = ustr_new();

    if (value_key_cell_get_pretty_output(in_vkc, in_pathname, out))
    {
        fprintf(stdout, "%s", ustr_as_utf8(out));
    }

    ustr_free(out);
}

gboolean
value_key_cell_get_pretty_output (ValueKeyCell *in_vkc,
                                  const char *in_pathname,
                                  ustring *in_output)
{
    if (in_vkc == NULL || in_output == NULL || in_pathname == NULL)
    {
        return FALSE;
    }

    Value *val = value_key_cell_get_val(in_vkc);

    ustr_printfa (in_output, _("Value %s\n"), in_pathname);

    ustring *val_name = value_key_cell_get_name(in_vkc);
    ustr_printfa (in_output, _("  Name: '%s'\n"), ustr_as_utf8(val_name));
    ustr_free(val_name);

    ustr_printfa (in_output, _("  Type: %s\n"), value_get_type_str(val));
    ustr_printfa (in_output, _("  Flag: %#06x\n"),
                  value_key_cell_get_flags(in_vkc));
    ustr_printfa (in_output, _("  Data Size (in value cell  ): %d\n"),
                  value_get_raw_length(val));
    ustr_printfa (in_output, _("  Data Size (in valuekeycell): %d\n"),
                  value_key_cell_get_data_size(in_vkc));
    if (value_get_raw_length(val) > 4096)
    {
        ustr_printfa (in_output, _("Data: Too large to print\n"));
    }
    else
    {
        ustring *val_str = value_get_as_string(val);
        ustr_printfa (in_output, _("  Data:\n%s\n"), ustr_as_utf8(val_str));
        ustr_free(val_str);
    }
    value_free(val);

    return TRUE;
}

gboolean
value_key_cell_get_xml_output (ValueKeyCell *in_vkc, ustring *in_output)
{
    if (in_vkc == NULL || in_output == NULL)
    {
        return FALSE;
    }

    ustr_printfa (in_output,
                  "<ValueKeyCell offset=\"%#010x\">\n",
                  offset_to_begin(get_offset(in_vkc)));

    ustr_printfa (in_output,
                  "  <name_length value=\"%d\"/>\n",
                  get_data(in_vkc)->len_name);
    ustr_printfa (in_output,
                  "  <data_length value=\"%d\" value_hex=\"%#06x\"/>\n",
                  get_data(in_vkc)->len_data,
                  get_data(in_vkc)->len_data);
    ustr_printfa (in_output,
                  "  <data_offset value=\"%#010x\"/>\n",
                  get_data(in_vkc)->ofs_data);
    ustr_printfa (in_output,
                  "  <value_type value=\"%#06x\" value_str=\"%s\"/>\n",
                  get_data(in_vkc)->val_type,
                  value_type_get_string(get_data(in_vkc)->val_type));
    ustr_printfa (in_output,
                  "  <flag value=\"%#06x\"/>\n",
                  get_data(in_vkc)->flag);
    guint16 dummy = get_data(in_vkc)->dummy1;
    ustr_printfa (in_output,
                  "  <dummy1 value=\"%#06x\", decivalue=\"%d\", "
                  "char=\"%c\"/>\n",
                  dummy, dummy, isprint(dummy) ? (char)dummy : '.');

    ustring *name = value_key_cell_get_name(in_vkc);
    ustr_printfa (in_output,
                  "  <name>%s</name>\n", ustr_as_utf8(name));
    ustr_free(name);
    {
        ValueCell *vc = value_key_cell_get_value_cell(in_vkc);
        if (vc != NULL)
        {
            value_cell_get_xml_output(vc, in_output);
        }
    }
    {
        Value *vc = value_key_cell_get_val(in_vkc);
        if (vc != NULL)
        {
            value_get_xml_output(vc, in_output);
        }
        value_free(vc);
    }

    ustr_printfa (in_output, "</ValueKeyCell>\n");
    return TRUE;
}

static gboolean
hcheck_one_vkc(KeyCell *in_kc, ValueKeyCell *in_vkc, RRACheckData *in_data)
{
    if (in_kc == NULL)
    {
        return FALSE;
    }

    if (!value_key_cell_is_valid(in_vkc))
    {
        rra_check_warning(in_data, _("invalid value key cell"));
        return FALSE;
    }

    gboolean ret_val = TRUE;

    rra_check_checking(in_data, _("for valid dword length"));
    // A DWORD should always have length 4.
    if (get_data(in_vkc)->val_type == REG_DWORD)
    {
        if (!is_inline_data_length(in_vkc))
        {
            rra_check_error(in_data,
                            _("dword type without inline length"));
            ret_val = FALSE;
        }
    }

    rra_check_checking(in_data, _("for valid string offset"));
    // A string with length of 0x80000004 means there is a single
    // character in the ofs_data field as the value.
    if (get_data(in_vkc)->val_type == REG_SZ
        || get_data(in_vkc)->val_type == REG_MULTI_SZ
        || get_data(in_vkc)->val_type == REG_EXPAND_SZ)
    {
        if (!is_inline_data_length(in_vkc)
            && !hive_is_valid_offset(get_hive(in_vkc),
                                     get_data(in_vkc)->ofs_data))
        {
            rra_check_error(in_data, _("Invalid offset"));
            ret_val = FALSE;
        }
    }

    rra_check_checking(in_data, _("for valid offsets"));
    if (get_data(in_vkc)->val_type == REG_BINARY
        || get_data(in_vkc)->val_type == REG_RESOURCE_LIST
        || get_data(in_vkc)->val_type == REG_FULL_RESOURCE_DESCRIPTOR
        || get_data(in_vkc)->val_type == REG_RESOURCE_REQUIREMENTS_LIST)
    {
        if (!is_inline_data_length(in_vkc)
            && !hive_is_valid_offset(get_hive(in_vkc),
                                     get_data(in_vkc)->ofs_data))
        {
            rra_check_error(in_data, _("Invalid offset"));
            ret_val = FALSE;
        }
    }

    rra_check_checking(in_data, _("for qword validity"));
    if (get_data(in_vkc)->val_type == REG_QWORD)
    {
        if (!hive_is_valid_offset(get_hive(in_vkc),
                                  get_data(in_vkc)->ofs_data))
        {
            rra_check_error(in_data, _("Invalid offset"));
            ret_val = FALSE;
        }

        if (get_data(in_vkc)->len_data != 8)
        {
            rra_check_error(in_data, _("Invalid data length for qword"));
            ret_val = FALSE;
        }
    }

    rra_check_checking(in_data, _("for invalid data length difference"));
    {
        ValueCell *vc = value_key_cell_get_value_cell(in_vkc);
        if (vc != NULL)
        {
            guint32 vc_len = value_cell_get_data_length(vc);
            guint32 vkc_len =
                (get_data((ValueKeyCell*)in_vkc)->len_data & 0x7fffffff);

            if (vc_len < vkc_len)
            {
                const guint8 *vc_data = value_key_cell_get_data(in_vkc);

                if (vc_len == 12
                    && vc_data[0] == (guint8)'d'
                    && vc_data[1] == (guint8)'b')
                {
                    rra_check_warning(in_data, _("Special 'db' value found."));
                }
                else
                {
                    rra_check_error(
                        in_data,
                        _("ValueCell length less than ValueKeyCell length: "
                          "%d vs %d"),
                        vc_len, vkc_len);
                    value_key_cell_debug_print(in_vkc);
                    ret_val = FALSE;
                }
            }
        }
    }

    return ret_val;
}

static gboolean
walk_tree_checker(KeyCell *in_kc, RRACheckData *in_data)
{
    if (in_kc == NULL)
    {
        rra_check_error(in_data, _("NULL key cell"));
        return FALSE;
    }

    guint i;

    gboolean ret_val = TRUE;

    for (i = 0; i < key_cell_get_number_of_values(in_kc); i++)
    {
        ValueKeyCell *vkc = key_cell_get_value(in_kc, i);
        if (!hcheck_one_vkc(in_kc, vkc, in_data))
        {
            ret_val = FALSE;
        }
    }

    /* walk into subkeys */
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
hcheck_value_key_cell_pass(Hive *in_hive, RRACheckData *in_data)
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
