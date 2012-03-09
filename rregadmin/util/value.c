/*
 * Authors:     James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi Inc
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
#include <errno.h>
#include <ctype.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/util/value.h>
#include <rregadmin/util/log.h>
#include <rregadmin/util/intutils.h>
#include <rregadmin/util/conversion_utils.h>
#include <rregadmin/util/malloc.h>

#define VALID_CHECK_RET(in_val, ret_val) \
    if (!value_is_valid(in_val))         \
    {                                    \
        rra_warning(N_("Invalid value")); \
        return (ret_val);                \
    }

/** Structure for value data.
 *
 * @ingroup value_util_group
 */
struct Value_
{
    /** The type of the value.
     */
    ValueType type;
    /** String type
     */
    UStringType str_type;
    /** The length of the data.
     */
    guint32 length;
    /** The raw data.
     */
    guint8 *raw_data;
    /** The value
     */
    union
    {
        qword_type qword_data;
        dword_type dword_data;
        // ustring *str_data;
        GPtrArray *arr_data;
    } value;
};

static Value* value_alloc(void);
static guint8* copy_binary(const guint8 *in_data, guint32 in_data_length);
static dword_type be_to_le(dword_type in_int);
static gboolean is_end_of_string_marker(const guint8 *buf_ptr,
                                        UStringType in_type);
static int get_type_increment(UStringType in_type);
static GPtrArray* copy_gptrarray(const GPtrArray *in_arr);

static int allocated_count = 0;
int
value_allocated_count(void)
{
    return allocated_count;
}

static guint8*
get_raw_data(Value *in_val)
{
    if (value_is_integral_type(in_val))
    {
        return (guint8*)&in_val->value.dword_data;
    }
    else if (value_is_binary_type(in_val))
    {
        return in_val->raw_data;
    }
    else
    {
        return NULL;
    }
}

gboolean
value_can_handle(ValueType in_type)
{
    if (in_type == REG_NONE
        || in_type == REG_SZ
        || in_type == REG_EXPAND_SZ
        || in_type == REG_MULTI_SZ
        || in_type == REG_BINARY
        || in_type == REG_DWORD
        || in_type == REG_DWORD_LITTLE_ENDIAN
        || in_type == REG_DWORD_BIG_ENDIAN
        || in_type == REG_QWORD
        || in_type == REG_QWORD_LITTLE_ENDIAN
        )
    {
        return TRUE;
    }

    return FALSE;
}

static Value*
value_alloc(void)
{
    Value *ret_val = rra_new_type(Value);
    ret_val->type = REG_NONE;
    ret_val->length = 0;
    ret_val->raw_data = NULL;
    ret_val->value.dword_data = 0;
    ret_val->str_type = USTR_TYPE_NONE;
    allocated_count++;

    return ret_val;
}

Value*
value_copy (const Value *in_val)
{
    Value *ret_val = value_alloc();
    ret_val->type = in_val->type;
    ret_val->str_type = in_val->str_type;
    ret_val->length = in_val->length;

    if (in_val->raw_data != NULL)
    {
        ret_val->raw_data = copy_binary(in_val->raw_data, in_val->length);
    }

    if (value_get_type(in_val) == REG_MULTI_SZ)
    {
        ret_val->value.arr_data = copy_gptrarray(in_val->value.arr_data);
    }
    else if (value_get_type(in_val) == REG_QWORD)
    {
        ret_val->value.qword_data = in_val->value.qword_data;
    }
    else
    {
        ret_val->value.dword_data = in_val->value.dword_data;
    }

    return ret_val;
}

gboolean
value_free(Value *in_val)
{
    VALID_CHECK_RET(in_val, FALSE);

    if (in_val->raw_data != NULL)
    {
        g_free(in_val->raw_data);
        in_val->raw_data = NULL;
    }

    if (value_get_type(in_val) == REG_MULTI_SZ)
    {
        if (in_val->value.arr_data != 0)
        {
            value_free_str_array(in_val->value.arr_data);
            in_val->value.arr_data = 0;
        }
    }

    rra_free_type(Value, in_val);

    allocated_count--;

    return TRUE;
}

gboolean
value_is_valid(const Value *in_val)
{
    if (in_val == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

int
value_compare(const Value *in_val1, const Value *in_val2)
{
    if (in_val1 == NULL)
    {
        if (in_val2 == NULL)
        {
            rra_debug("Val1 (%p) and Val2 (%p) are NULL",
                      in_val1, in_val2);
            return 0;
        }
        else
        {
            rra_debug("Val1 (%p) is NULL", in_val1);
            return -1;
        }
    }
    else
    {
        if (in_val2 == NULL)
        {
            rra_debug("Val2 is NULL");
            return 1;
        }
    }

    if (value_is_integral_type(in_val1)
        && value_is_integral_type(in_val2))
    {
        qword_type v1 = value_get_as_qword(in_val1);
        qword_type v2 = value_get_as_qword(in_val2);
        rra_debug("Comparing integrals: %s vs %s", v1, v2);
        return v1 - v2;
    }

    if (value_is_string_type(in_val1)
        && value_is_string_type(in_val2))
    {
        ustring *str1 = value_get_as_string(in_val1);
        ustring *str2 = value_get_as_string(in_val2);
        int ret_val = ustr_compare(str1, str2);
        ustr_free(str1);
        ustr_free(str2);
        return ret_val;
    }

    GByteArray *arr1 = value_encode(in_val1, VALUE_STR_UTF16);
    GByteArray *arr2 = value_encode(in_val2, VALUE_STR_UTF16);

    int ret_val = 0;
    guint i;
    int len = arr1->len > arr2->len ? arr2->len : arr1->len;
    int def_ret_val = arr1->len - arr2->len;

    for (i = 0; i < len; i++)
    {
        if (arr1->data[i] != arr2->data[i])
        {
            rra_debug("byte %d differs: %c vs %c",
                      i, arr1->data[i], arr2->data[i]);
            ret_val = arr1->data[i] - arr2->data[i];
            break;
        }
    }

    g_byte_array_free(arr1, TRUE);
    g_byte_array_free(arr2, TRUE);

    if (ret_val == 0)
    {
        return def_ret_val;
    }
    else
    {
        return ret_val;
    }
}

gboolean
value_equal(const Value *in_val1, const Value *in_val2)
{
    return value_compare(in_val1, in_val2) == 0;
}

Value*
value_create_none(void)
{
    Value *ret_val = value_alloc();
    ret_val->type = REG_NONE;
    ret_val->length = 0;
    ret_val->value.dword_data = 0;
    return ret_val;
}

Value *
value_create_dword (dword_type in_int)
{
    Value *ret_val = value_alloc();
    ret_val->type = REG_DWORD;
    ret_val->length = 4;
    ret_val->value.dword_data = in_int;
    return ret_val;
}

static dword_type
be_to_le(dword_type in_int)
{
    return GUINT32_SWAP_LE_BE_CONSTANT(in_int);
}

Value*
value_create_dword_be (dword_type in_int)
{
    Value *ret_val = value_alloc();
    ret_val->type = REG_DWORD_BIG_ENDIAN;
    ret_val->length = 4;
    ret_val->value.dword_data = be_to_le(in_int);
    return ret_val;
}

Value*
value_create_qword (qword_type in_int)
{
    Value *ret_val = value_alloc();
    ret_val->type = REG_QWORD;
    ret_val->length = 8;
    ret_val->value.qword_data = in_int;
    return ret_val;
}

Value*
value_create_binary(const guint8 *data, guint32 data_length)
{
    Value *ret_val = value_alloc();
    ret_val->type = REG_BINARY;
    ret_val->length = data_length;
    ret_val->raw_data = copy_binary(data, data_length);
    return ret_val;
}

Value*
value_create_from_binary(const guint8 *data, guint32 data_length,
                         ValueType in_type)
{
    if (in_type == REG_NONE)
    {
        return value_create_none();
    }

    if (data == NULL)
    {
        if (value_type_is_binary(in_type))
        {
            Value *ret_val = value_create_binary(data, data_length);
            ret_val->type = in_type;
            return ret_val;
        }
        else
        {
            return NULL;
        }
    }

    if (in_type == REG_DWORD)
    {
        if (data_length != 4)
        {
            return NULL;
        }
        return value_create_dword((dword_type)*data);
    }
    else if (in_type == REG_DWORD_BIG_ENDIAN)
    {
        if (data_length != 4)
        {
            return NULL;
        }
        return value_create_dword_be((dword_type)*data);
    }
    else if (in_type == REG_QWORD)
    {
        if (data_length != 8)
        {
            return NULL;
        }
        return value_create_qword((qword_type)*data);
    }
    else if (in_type == REG_SZ)
    {
        return value_create_string((const char*)data, data_length,
                                   USTR_TYPE_UTF8);
    }
    else if (in_type == REG_MULTI_SZ)
    {
        return value_create_multi_string_bin((const char*)data, data_length,
                                             USTR_TYPE_UTF8);
    }
    else if (in_type == REG_EXPAND_SZ)
    {
        return value_create_expanded_string((const char*)data, data_length,
                                            USTR_TYPE_UTF8);
    }
    else
    {
        Value *ret_val = value_create_binary(data, data_length);
        ret_val->type = in_type;
        return ret_val;
    }
}

Value*
value_create_from_string(const gchar *in_str, ValueType in_type)
{
    if (in_type == REG_NONE)
    {
        return value_create_none();
    }
    else if (in_type == REG_DWORD)
    {
        if (in_str == NULL)
        {
            return NULL;
        }

        if (strlen(in_str) == 0)
        {
            return NULL;
        }

        dword_type val;
        if (!str_to_guint32(in_str, &val))
        {
            return NULL;
        }
        return value_create_dword(val);
    }
    else if (in_type == REG_QWORD)
    {
        if (in_str == NULL)
        {
            return NULL;
        }

        if (strlen(in_str) == 0)
        {
            return NULL;
        }

        qword_type val;
        if (!str_to_gint64(in_str, &val))
        {
            return NULL;
        }
        return value_create_qword(val);
    }
    else if (in_type == REG_SZ)
    {
        if (in_str == NULL)
        {
            return NULL;
        }

        return value_create_string(in_str, strlen(in_str) + 1,
                                   USTR_TYPE_UTF8);
    }
    else if (in_type == REG_MULTI_SZ)
    {
        if (in_str == NULL)
        {
            return NULL;
        }

        return value_create_multi_string(in_str, strlen(in_str),
                                         USTR_TYPE_UTF8);
    }
    else if (in_type == REG_EXPAND_SZ)
    {
        if (in_str == NULL)
        {
            return NULL;
        }
        return value_create_expanded_string(in_str, strlen(in_str) + 1,
                                            USTR_TYPE_UTF8);
    }
    else if (value_type_is_binary(in_type))
    {
        GByteArray *arr = g_byte_array_new();
        if (!str_to_binary(in_str, arr))
        {
            rra_message(N_("str_to_binary failed for %s"), in_str);
            g_byte_array_free(arr, TRUE);
            return NULL;
        }

        Value *ret_val = value_create_from_binary(arr->data, arr->len,
                                                  in_type);
        g_byte_array_free(arr, TRUE);
        return ret_val;
    }
    else
    {
        return NULL;
    }
}

Value*
value_create_string(const char *in_str, guint32 in_str_len,
                    UStringType in_type)
{
    Value *ret_val = value_alloc();
    ret_val->type = REG_SZ;
    ret_val->str_type = in_type;
    ret_val->raw_data = copy_binary((const guint8*)in_str, in_str_len);
    ret_val->length = in_str_len;
    return ret_val;
}

Value*
value_create_expanded_string(const char *in_str, guint32 in_str_len,
                             UStringType in_type)
{
    Value *ret_val = value_create_string(in_str, in_str_len, in_type);
    ret_val->type = REG_EXPAND_SZ;
    return ret_val;
}

static gboolean
is_end_of_string_marker(const guint8 *buf_ptr, UStringType in_type)
{
    if (in_type == USTR_TYPE_UTF16LE)
    {
        return (buf_ptr[0] == '\0' && buf_ptr[1] == '\0');
    }
    else
    {
        return buf_ptr[0] == '\0';
    }
}

static int
get_type_increment(UStringType in_type)
{
    if (in_type == USTR_TYPE_UTF16LE)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

Value*
value_create_multi_string(const char *in_str, guint32 in_str_len,
                          UStringType in_type)
{
    Value *ret_val = value_alloc();
    ret_val->type = REG_MULTI_SZ;
    ret_val->value.arr_data = g_ptr_array_new();

    ustring *new_str = ustr_create_type(in_str, in_str_len, in_type);
    ret_val->length = ustr_size(new_str);

    rra_info(N_("Multi string '%s' found"), ustr_as_utf8(new_str));

    g_ptr_array_add(ret_val->value.arr_data, new_str);

    return ret_val;
}

Value*
value_create_multi_string_bin(const char *in_str, guint32 in_str_len,
                              UStringType in_type)
{
    gboolean good_parse = FALSE;

    const guint8 *buf_ptr = (guint8*)in_str;

    Value *ret_val = value_alloc();
    ret_val->type = REG_MULTI_SZ;
    ret_val->length = in_str_len;
    ret_val->value.arr_data = g_ptr_array_new();

    while (TRUE)
    {
        const guint8 *end_ptr;

        if (is_end_of_string_marker(buf_ptr, in_type))
        {
            if ((buf_ptr - (guint8*)in_str + 1) == (gint32)in_str_len)
            {
                good_parse = TRUE;
            }
            else
            {
                // This happens too often to print out a warning on each.
                rra_debug("Didn't get to end of data diff: %d",
                          buf_ptr - (const guint8*)in_str);
		// This seems to be ok.
                good_parse = TRUE;
            }
            break;
        }

        end_ptr = buf_ptr;
        while (!is_end_of_string_marker(end_ptr, in_type)
               && (end_ptr - (guint8*)in_str) < (gint32)in_str_len)
        {
            end_ptr += get_type_increment(in_type);
        }

        if (end_ptr - buf_ptr == 0)
        {
            good_parse = TRUE;
            break;
        }

        ustring *new_str = ustr_create_type((char*)buf_ptr,
                                            end_ptr - buf_ptr,
                                            in_type);

        if (new_str == NULL)
        {
            rra_warning(N_("Got NULL from ustr_create_type"));
            break;
        }
        else
        {
            g_ptr_array_add(ret_val->value.arr_data, new_str);
        }

        buf_ptr = end_ptr;

        if ((buf_ptr - (guint8*)in_str) == (gint32)in_str_len)
        {
            // Multi strings should end with double string enders.
            // So for a ascii string it'd look like:
            // "foo\0bar\0baz\0\0" But that didn't happen this time.
            // Something minor is wrong in the hive.
            rra_message(N_("Got to end of data without double nulls"));
            // Since it can happen in basically good hives we won't
            // mark it as an error.
            good_parse = TRUE;
            break;
        }

        buf_ptr += get_type_increment(in_type);
    }

    if (!good_parse)
    {
        value_free(ret_val);
        return NULL;
    }

    return ret_val;
}

gboolean
value_is_integral_type(const Value *in_val)
{
    if (in_val == NULL)
    {
        return FALSE;
    }

    return value_type_is_integral(value_get_type(in_val));
}

gboolean
value_is_string_type(const Value *in_val)
{
    if (in_val == NULL)
    {
        return FALSE;
    }

    return value_type_is_string(value_get_type(in_val));
}

gboolean
value_is_binary_type(const Value *in_val)
{
    if (in_val == NULL)
    {
        return FALSE;
    }

    return value_type_is_binary(value_get_type(in_val));
}

gboolean
value_clear_bits(Value *in_val, guint32 in_bytenum, guint8 in_mask)
{
    if (in_val == NULL)
    {
        return FALSE;
    }

    if (!value_is_binary_type(in_val)
        && !value_is_integral_type(in_val))
    {
        return FALSE;
    }

    if (in_bytenum >= value_get_raw_length(in_val))
    {
        return FALSE;
    }

    guint8 *data = get_raw_data(in_val);

    data[in_bytenum] &= ~in_mask;

    return TRUE;
}

gboolean
value_set_bits(Value *in_val, guint32 in_bytenum, guint8 in_mask)
{
    if (in_val == NULL)
    {
        return FALSE;
    }

    if (!value_is_binary_type(in_val)
        && !value_is_integral_type(in_val))
    {
        return FALSE;
    }

    if (in_bytenum >= value_get_raw_length(in_val))
    {
        return FALSE;
    }

    guint8 *data = get_raw_data(in_val);

    data[in_bytenum] |= in_mask;

    return TRUE;
}

guint32
value_get_raw_length(const Value *in_val)
{
    VALID_CHECK_RET(in_val, 0);
    return in_val->length;
}

ValueType
value_get_type(const Value *in_val)
{
    VALID_CHECK_RET(in_val, REG_NONE);
    return in_val->type;
}

const char*
value_get_type_str(const Value *in_val)
{
    VALID_CHECK_RET(in_val, "none");
    return value_type_get_string(value_get_type(in_val));
}

dword_type
value_get_as_dword(const Value *in_val)
{
    VALID_CHECK_RET(in_val, 0);
    if (value_get_type(in_val) == REG_DWORD
        || value_get_type(in_val) == REG_DWORD_BIG_ENDIAN)
    {
        return in_val->value.dword_data;
    }
    else if (value_get_type(in_val) == REG_QWORD)
    {
        qword_type tmp_val = value_get_as_qword(in_val);
        if (tmp_val > G_MININT32 && tmp_val < G_MAXINT32)
        {
            return (dword_type)tmp_val;
        }
        else if (tmp_val > 0)
        {
            return G_MAXINT32;
        }
        else
        {
            return G_MININT32;
        }
    }
    else
    {
        return 0;
    }
}

qword_type
value_get_as_qword(const Value *in_val)
{
    VALID_CHECK_RET(in_val, 0);
    if (value_is_integral_type(in_val))
    {
        if (value_get_type(in_val) == REG_QWORD)
        {
            return in_val->value.qword_data;
        }
        else
        {
            return in_val->value.dword_data;
        }
    }
    else
    {
        return 0;
    }
}

ustring *
value_get_as_string(const Value *in_val)
{
    VALID_CHECK_RET(in_val, NULL);

    {
        ustring *ret_val = ustr_new();

        if (value_get_type(in_val) == REG_NONE)
        {
            ustr_strset(ret_val, "");
        }
        else if (value_get_type(in_val) == REG_DWORD
            || value_get_type(in_val) == REG_DWORD_BIG_ENDIAN)
        {
            ustr_printf(ret_val, "%u", value_get_as_dword(in_val));
        }
        else if (value_get_type(in_val) == REG_QWORD)
        {
            ustr_printf(ret_val, "%" G_GINT64_FORMAT, value_get_as_qword(in_val));
        }
        else if (value_is_binary_type(in_val))
        {
            ustr_hexdump(ret_val, in_val->raw_data, 0,
                         in_val->length, TRUE, TRUE);
        }
        else if (value_get_type(in_val) == REG_SZ
                 || value_get_type(in_val) == REG_EXPAND_SZ)
        {
            if (in_val->length > 0)
            {
                ustr_free(ret_val);
                ret_val = ustr_create_type((const char*)in_val->raw_data,
                                           in_val->length, in_val->str_type);
            }
        }
        else if (value_get_type(in_val) == REG_MULTI_SZ)
        {
            int i;
            int end = in_val->value.arr_data->len;
            for (i = 0; i < end; i++)
            {
                ustr_strcat(ret_val,
                            ustr_as_utf8(
                                (const ustring*)g_ptr_array_index(in_val->value.arr_data, i)));
                if ((i + 1) < end)
                {
                    ustr_strcat(ret_val, ", ");
                }
            }
        }
        else
        {
            GByteArray *arr = value_encode(in_val, 0);
            ustr_hexdump(ret_val, arr->data, 0, arr->len, TRUE, TRUE);
            g_byte_array_free(arr, TRUE);
        }

        return ret_val;
    }
}

ustring*
value_get_as_hexstring(const Value *in_val)
{
    VALID_CHECK_RET(in_val, NULL);
    {
        GByteArray *arr_rep = value_encode(in_val, 0);
        ustring *ret_val = ustr_new();
        ustr_strset(ret_val, "");

        ustr_hexdump(ret_val, arr_rep->data, 0, arr_rep->len, TRUE, TRUE);

        g_byte_array_free(arr_rep, TRUE);

        return ret_val;
    }
}

guint8*
value_get_as_binary(const Value *in_val)
{
    VALID_CHECK_RET(in_val, NULL);
    {
        GByteArray *tmp_ba;
        guint8 *ret_val;

        tmp_ba = value_encode(in_val, 0);

        ret_val = copy_binary(tmp_ba->data, tmp_ba->len);

        g_byte_array_free(tmp_ba, TRUE);

        return ret_val;
    }
}

static guint8*
copy_binary(const guint8 *in_data, guint32 in_data_length)
{
    guint8 *ret_val = g_new(guint8, in_data_length);
    memcpy(ret_val, in_data, in_data_length);
    return ret_val;
}

GByteArray*
value_encode(const Value *in_val, guint32 in_encode_flags)
{
    VALID_CHECK_RET(in_val, NULL);

    {
        GByteArray *ret_val = g_byte_array_new();

        if (value_get_type(in_val) == REG_DWORD)
        {
            g_byte_array_set_size(ret_val, in_val->length);
            memcpy(ret_val->data, (guint8*)&in_val->value.dword_data,
                   in_val->length);
        }
        else if (value_get_type(in_val) == REG_DWORD_BIG_ENDIAN)
        {
            guint32 be_int =
                GUINT32_SWAP_LE_BE_CONSTANT(in_val->value.dword_data);

            g_byte_array_set_size(ret_val, in_val->length);
            memcpy(ret_val->data, (guint8*)&be_int,
                   in_val->length);
        }
        else if (value_get_type(in_val) == REG_QWORD)
        {
            g_byte_array_set_size(ret_val, in_val->length);
            memcpy(ret_val->data, (guint8*)&in_val->value.qword_data,
                   in_val->length);
        }
        else if (value_is_binary_type(in_val))
        {
            g_byte_array_set_size(ret_val, in_val->length);
            memcpy(ret_val->data, in_val->raw_data,
                   in_val->length);
        }
        else if (value_is_string_type(in_val))
        {
            UStringType tmp_type = USTR_TYPE_UTF8;
            int offset = 1;
            if (in_encode_flags & VALUE_STR_UTF16)
            {
                tmp_type = USTR_TYPE_UTF16LE;
                offset = 2;
            }
            else if (in_encode_flags & VALUE_STR_ASCII)
            {
                tmp_type = USTR_TYPE_ASCII;
                offset = 1;
            }
            else if (in_encode_flags & VALUE_STR_ISO8859_1)
            {
                tmp_type = USTR_TYPE_ISO8859_1;
                offset = 1;
            }

            if (value_get_type(in_val) == REG_SZ
                || value_get_type(in_val) == REG_EXPAND_SZ)
            {
                if (tmp_type == in_val->str_type)
                {
                    g_byte_array_set_size(ret_val, in_val->length);
                    memcpy(ret_val->data, in_val->raw_data,
                           in_val->length);
                }
                else
                {
                    g_byte_array_free(ret_val, TRUE);
                    ustring *tmp_str =
                        ustr_create_type((const char *)in_val->raw_data,
                                         in_val->length, in_val->str_type);
                    ret_val = ustr_encode(tmp_str, tmp_type);
                    ustr_free(tmp_str);
                }
            }
            else if (value_get_type(in_val) == REG_MULTI_SZ)
            {
                int i;
                int end = in_val->value.arr_data->len;

                for (i = 0; i < end; i++)
                {
                    GByteArray *tmp_arr =
                        ustr_encode(
                            (const ustring*)g_ptr_array_index(in_val->value.arr_data, i),
                            tmp_type);
                    ret_val = g_byte_array_append(
                        ret_val, tmp_arr->data, tmp_arr->len);
                    g_byte_array_free(tmp_arr, TRUE);
                }
                if (offset >= 1)
                {
                    g_byte_array_append(ret_val, (guint8*)"\0", 1);
                }
                if (offset >= 2)
                {
                    g_byte_array_append(ret_val, (guint8*)"\0", 1);
                }
            }
            else
            {
                g_assert_not_reached();
            }
        }
        else if (value_get_type(in_val) == REG_NONE)
        {
            /* don't do anything */
        }
        else
        {
            g_assert_not_reached();
        }

        return ret_val;
    }
}

static GPtrArray*
copy_gptrarray(const GPtrArray *in_arr)
{
    int i;
    int end;
    GPtrArray *ret_val = g_ptr_array_new();

    end = in_arr->len;
    for (i = 0; i < end; i++)
    {
        g_ptr_array_add(
            ret_val,
            ustr_copy((const ustring*)g_ptr_array_index(in_arr, i)));
    }

    return ret_val;
}

GPtrArray*
value_get_as_string_array(const Value *in_val)
{
    VALID_CHECK_RET(in_val, NULL);

    if (value_get_type(in_val) == REG_MULTI_SZ)
    {
        return copy_gptrarray(in_val->value.arr_data);
    }
    else
    {
        GPtrArray *ret_val = g_ptr_array_new();
        ustring *tmp_str = value_get_as_string(in_val);
        g_ptr_array_add(ret_val, tmp_str);
        return ret_val;
    }
}

gboolean
value_free_str_array(GPtrArray *in_arr)
{
    guint32 i;

    if (in_arr == NULL)
    {
        return FALSE;
    }

    for (i = 0; i < in_arr->len; i++)
    {
        ustr_free((ustring*)g_ptr_array_index(in_arr, i));
    }
    g_ptr_array_free(in_arr, TRUE);
    return TRUE;
}

void
value_debug_print (const Value *in_val)
{
    ustring *out = ustr_new();

    if (value_get_xml_output(in_val, out))
    {
        fprintf(stderr, "%s", ustr_as_utf8(out));
    }

    ustr_free(out);
}

gboolean
value_get_xml_output(const Value *in_val, ustring *in_output)
{
    if (in_val == NULL || in_output == NULL)
    {
        return FALSE;
    }

    ustr_printfa (in_output,  "<Value>\n");
    ustr_printfa (in_output,  "  <type value=\"%d\" value_str=\"%s\"/>\n",
             value_get_type(in_val), value_get_type_str(in_val));
    ustr_printfa (in_output,  "  <length value=\"%d\"/>\n",
             value_get_raw_length(in_val));
    ustr_printfa (in_output,  "  <value>\n");
    ustring *as_str = value_get_as_string(in_val);
    ustr_printfa (in_output,  "%s\n", ustr_as_utf8(as_str));
    ustr_free(as_str);
    ustr_printfa (in_output,  "  </value>\n");
    ustr_printfa (in_output,  "</Value>\n");

    return TRUE;
}
