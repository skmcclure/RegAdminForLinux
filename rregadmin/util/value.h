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

#ifndef RREGADMIN_UTIL_VALUE_H
#define RREGADMIN_UTIL_VALUE_H 1

#include <glib/gtypes.h>
#include <glib/garray.h>

#include <rregadmin/util/value_type.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/util/ustring.h>

G_BEGIN_DECLS

/**
 * @defgroup value_util_group Value Object
 *
 * @ingroup util_group
 */

typedef struct Value_ Value;

/** Return whether the value implementation handles a particular type.
 *
 * @ingroup value_util_group
 */
gboolean value_can_handle (ValueType in_type);

/** Create an empty value.
 *
 * @ingroup value_util_group
 */
Value* value_create_none (void);

/** Create a dword value.
 *
 * @ingroup value_util_group
 */
Value* value_create_dword (dword_type in_int);

/** Create a dword value from big endian data.
 *
 * @ingroup value_util_group
 */
Value* value_create_dword_be (dword_type in_int);

/** Create a qword value.
 *
 * @ingroup value_util_group
 */
Value* value_create_qword (qword_type in_int);

/** Create a binary value.
 *
 * @ingroup value_util_group
 */
Value* value_create_binary (const guint8 *data, guint32 data_length);

/** This provides support for loading from binary data.
 *
 * @ingroup value_util_group
 */
Value* value_create_from_binary (const guint8 *data, guint32 data_length,
                                 ValueType in_type);

/** This provides smart support for loading from a string.
 *
 * It can currently load strings, dwords, qwords, and specially
 * formatted binary data.
 *
 * @ingroup value_util_group
 */
Value* value_create_from_string (const gchar *data, ValueType in_type);

/** Create a string value.
 *
 * @ingroup value_util_group
 */
Value* value_create_string (const char *in_str, guint32 in_str_len,
                            UStringType in_type);

/** Create an expanded string value.
 *
 * @ingroup value_util_group
 */
Value* value_create_expanded_string (const char *in_str, guint32 in_str_len,
                                     UStringType in_type);
/** Create a multi string value.
 *
 * @ingroup value_util_group
 */
Value* value_create_multi_string_bin (const char *in_str,
                                      guint32 in_str_len,
                                      UStringType in_type);

/** Create a multi string value from a string
 *
 * @ingroup value_util_group
 */
Value* value_create_multi_string (const char *in_str,
                                  guint32 in_str_len,
                                  UStringType in_type);

/** Copy a value
 *
 * @ingroup value_util_group
 */
Value* value_copy (const Value *in_val);

/** Free a value.
 *
 * @ingroup value_util_group
 */
gboolean value_free (Value *in_val);

/** Check the equality of values.
 *
 * @ingroup value_util_group
 */
gboolean value_equal (const Value *in_val1, const Value *in_val2);

/** Check the equality of values.
 *
 * @ingroup value_util_group
 */
int value_compare (const Value *in_val1, const Value *in_val2);

/** Check the validity of the value.
 *
 * You should never need to call this since creation and modification
 * of values is tightly controlled.
 *
 * @ingroup value_util_group
 */
gboolean value_is_valid (const Value *in_val);

/** Is this value an integral type?
 *
 * @ingroup value_util_group
 */
gboolean value_is_integral_type (const Value *in_val);

/** Is this value a string type?
 *
 * @ingroup value_util_group
 */
gboolean value_is_string_type (const Value *in_val);

/** Is this value a binary type?
 *
 * @ingroup value_util_group
 */
gboolean value_is_binary_type (const Value *in_val);

/** Apply a clear mask to the specified byte in the value.
 *
 * This is one of a very few methods that allow you to modify a Value
 * after it has been created.
 *
 * This method is only functional for integral and binary values.  All
 * others will fail.
 *
 * @ingroup value_util_group
 */
gboolean value_clear_bits (Value *in_val, guint32 in_bytenum, guint8 in_mask);

/** Apply a set mask to the specified byte in the value.
 *
 * This is one of a very few methods that allow you to modify a Value
 * after it has been created.
 *
 * This method is only functional for integral and binary values.  All
 * others will fail.
 *
 * @ingroup value_util_group
 */
gboolean value_set_bits (Value *in_val, guint32 in_bytenum, guint8 in_mask);

/** Get the raw length of the value.
 *
 * @ingroup value_util_group
 */
guint32 value_get_raw_length (const Value *in_val);

/** Get the type of the value.
 *
 * @ingroup value_util_group
 */
ValueType value_get_type (const Value *in_val);

/** Get a string rep of the type of the value.
 *
 * @ingroup value_util_group
 */
const char* value_get_type_str (const Value *in_val);

/** Get the value as a dword.
 *
 * For dwords and be dwords this does the expected thing.
 *
 * For qwords if the value is within the range of a dword it is converted
 * to a dword and returned.  If the value is larger than a dword can
 * contain then a MAXINT32 is returned.  If the value is smaller: MININT32.
 *
 * For any other type zero is returned.
 *
 * @ingroup value_util_group
 */
dword_type value_get_as_dword (const Value *in_val);

/** Get the value as a qword.
 *
 * For dwords, be dwords and qwords this does the expected thing.
 *
 * For any other type zero is returned.
 *
 * @ingroup value_util_group
 */
qword_type value_get_as_qword (const Value *in_val);

/** Get the value as a user friendly string.
 *
 * \note You must free the return with ustr_free
 *
 * @ingroup value_util_group
 */
ustring* value_get_as_string (const Value *in_val);

/** Get as a hex string.
 *
 * \note You must free the return with ustr_free
 *
 * @ingroup value_util_group
 */
ustring* value_get_as_hexstring (const Value *in_val);

/** Get binary data for the value.
 *
 * \note You must free the return with ustr_free
 *
 * @ingroup value_util_group
 */
guint8* value_get_as_binary (const Value *in_val);


typedef enum {
    VALUE_STR_UTF8 = 1 << 0,
    VALUE_STR_UTF16 = 1 << 1,
    VALUE_STR_ASCII = 1 << 2,
    VALUE_STR_ISO8859_1 = 1 << 3
} encode_flags;

/** Encode the value in an appropriate way for a hive file.
 *
 * \note You must call g_byte_array_free on the returned value.
 *
 * @ingroup value_util_group
 */
GByteArray* value_encode (const Value *in_val, guint32 in_encode_flags);

/** Get the value as an array of ustrings.
 *
 * Please use value_free_str_array to free the GPtrArray so the
 * ustrings get destructed as well.
 *
 * @ingroup value_util_group
 */
GPtrArray* value_get_as_string_array (const Value *in_val);

/** Destroy a GPtrArray returned from value_get_as_string_array.
 *
 * @ingroup value_util_group
 */
gboolean value_free_str_array (GPtrArray *in_arr);

/** Get an xml representation of the value.
 *
 * @ingroup value_util_group
 */
gboolean value_get_xml_output (const Value *in_val, ustring *in_output);

/** Print an xml representation of the value to stderr.
 *
 * @ingroup value_util_group
 */
void value_debug_print (const Value *in_val);

/** Return number of allocated value objects.
 *
 * @internal for testing purposes
 *
 * @ingroup value_util_group
 */
int value_allocated_count (void);

G_END_DECLS

#endif // RREGADMIN_UTIL_VALUE_H
