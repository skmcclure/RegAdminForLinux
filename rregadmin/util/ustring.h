/*
 * Authors:	Sean Loaring
 *		Petter Nordahl-Hagen
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


#ifndef RREGADMIN_UTIL_USTRING_H
#define RREGADMIN_UTIL_USTRING_H 1

#include <stdlib.h>
#include <stdarg.h>

#include <glib.h>

#include <rregadmin/util/macros.h>

G_BEGIN_DECLS

/**
 * @defgroup ustring_util_group UString Object
 *
 * @ingroup util_group
 */

/** String type typedef.
 *
 * @ingroup ustring_util_group
 */
typedef enum
{
    /** Unknown or absent string type.
     *
     * @ingroup ustring_util_group
     */
    USTR_TYPE_NONE = 0,

    /** UTF-8 string type.
     *
     * @ingroup ustring_util_group
     */
    USTR_TYPE_UTF8 = 10,

    /** UTF-16LE string type.
     *
     * @ingroup ustring_util_group
     */
    USTR_TYPE_UTF16LE = 20,

    /** ASCII string type.
     *
     * @ingroup ustring_util_group
     */
    USTR_TYPE_ASCII = 30,

    /** ISO8859-1 string type.
     *
     * @ingroup ustring_util_group
     */
    USTR_TYPE_ISO8859_1 = 40,

} UStringType;

typedef struct ustring_ ustring;

/** Create an empty ustring.  Equivalent to ustr_alloc(0).
 *
 * @ingroup ustring_util_group
 */
ustring* ustr_new(void) G_GNUC_MALLOC;


/** Create a new ustring from a const char*
 *
 * @ingroup ustring_util_group
 */
ustring* ustr_create(const char *in_str) G_GNUC_MALLOC;

/** Create a new ustring from a const char* and a type id.
 *
 * @ingroup ustring_util_group
 */
ustring* ustr_create_type(const char *in_str, guint32 in_str_len,
                           UStringType in_type) G_GNUC_MALLOC;

/** Create a copy of a ustring.
 *
 * @ingroup ustring_util_group
 */
ustring* ustr_copy (const ustring *in_ustr) G_GNUC_MALLOC;

/** Check to see if this is a valid ustring
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_is_valid (const ustring *in_ustr) G_GNUC_PURE;

/** Free a dynamically created ustring.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_free (ustring *in_ustr);

/** overwrite ustrdest with ustrsrc
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_overwrite (ustring *in_ustrdest, const ustring *in_ustrsrc);

/** Get the number of bytes currently stored.
 *
 * @ingroup ustring_util_group
 */
int ustr_size (const ustring *in_ustr) G_GNUC_PURE;

/** Get the length of the string in characters.
 *
 * \bug This doesn't actually do this now.
 *
 * @ingroup ustring_util_group
 */
int ustr_strlen(const ustring *in_ustr) G_GNUC_PURE;

/** Compare two ustrings taking into account utf8 characters.
 *
 * @ingroup ustring_util_group
 */
int ustr_compare(const ustring *in_ustra, const ustring *in_ustrb);

/** Compare two ustring like strcasecmp.
 *
 * @ingroup ustring_util_group
 */
int ustr_casecmp(const ustring *in_ustra, const ustring *in_ustrb);

/** Compare strings in the way a registry does.
 *
 * @ingroup ustring_util_group
 */
int ustr_regcmp(const ustring *in_ustra, const ustring *in_ustrb);

/** Check to see if two ustrings are equal.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_equal(const ustring *in_ustra, const ustring *in_ustrb);

/** Compare a ustring and a char string.
 *
 * @ingroup ustring_util_group
 */
int ustr_compare_str(const ustring *in_ustra, const char *in_str);

/** Compare a ustring and a char string like strcasecmp.
 *
 * @ingroup ustring_util_group
 */
int ustr_casecmp_str(const ustring *in_ustra, const char *in_str);

/** Check to see if a ustring and a char string are equal.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_equal_str(const ustring *in_ustra, const char *in_str);

/** Encode the data in the particular format and return in a GByteArray
 * (minus any string endings like \\0)
 *
 * You must call g_byte_array_free on the returned value.
 *
 * @ingroup ustring_util_group
 */
GByteArray* ustr_encode (const ustring *in_ustr, UStringType in_type)
    G_GNUC_MALLOC;

/** Encode the data in the particular format represented by in_code_name.
 *
 * (see iconv for format possibilities) and return in a GByteArray
 * (minus any string endings like \\0)
 * You must call g_byte_array_free on the returned value.
 *
 * @ingroup ustring_util_group
 */
GByteArray* ustr_generic_encode(const ustring *in_ustr,
                                const char *in_code_name)
     G_GNUC_MALLOC;

/** Get the ustr data as utf8 (the default storage encoding)
 *
 * @ingroup ustring_util_group
 */
const char* ustr_as_utf8 (const ustring *in_ustr) G_GNUC_PURE;

/** Get the char located at in_index.
 *
 * @ingroup ustring_util_group
 */
gunichar ustr_index(const ustring *in_ustr, gssize in_index);

/** Get the char located at in_index.
 *
 * @warning No bounds checking is done.
 *
 * @ingroup ustring_util_group
 */
gunichar ustr_index_unsafe(const ustring *in_ustr, gssize in_index);

/** returns the last character stored in the ustring
 *
 * @ingroup ustring_util_group
 */
int ustr_last_char (const ustring *in_ustr) G_GNUC_PURE;

/** Clear the ustring of any data.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_clear (ustring *in_ustr);

/** trims the first skip characters
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_trim_front (ustring *in_ustr, size_t skip);

/** trims the last skip characters
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_trim_back (ustring *in_ustr, size_t skip);

/** trims all begining whitespace
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_trim_ws_front (ustring *in_ustr);

/** trims all ending whitespace
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_trim_ws_back (ustring *in_ustr);

/** trims all whitespace at the front and back
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_trim_ws (ustring *in_ustr);

/** Sets all the characters in the ustring to lowercase.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_strdown(ustring *in_ustr);

/** Sets all the characters in the ustring to uppercase.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_strup(ustring *in_ustr);

/** Append string in_str into ustring wrapping at in_column and prepending
 * in_prefix to each line.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_wrapa(ustring *in_ustr, const char *in_str,
                    const char *in_prefix, int in_column);

/** Insert string in_str into ustring, clearing all other data, wrapping
 * at in_column and prepending in_prefix to each line.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_wrap(ustring *in_ustr, const char *in_str,
                   const char *in_prefix, int in_column);

/** Like sprintf() but print to the ustring.
 *
 * This overwrites any data in the ustring.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_printf (ustring *in_ustr, const char *fmt, ...)
    G_GNUC_PRINTF(2, 3);

/** Like ustr_printf(), but appends the new data.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_printfa (ustring *in_ustr, const char *fmt, ...)
    G_GNUC_PRINTF(2, 3);

/** Like vsprintf() but print to the ustring.
 *
 * This overwrites any data in the ustring.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_vprintf (ustring *in_ustr, const char *fmt, va_list args);

/** Like ustr_vprintf(), but appends the new data.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_vprintfa (ustring *in_ustr, const char *fmt, va_list args);


/** Create a string representation of a hexdump of the buffer in the
 * ustring.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_hexdump (ustring *in_ustr, const guint8 *buffer,
                       int start, int stop, gboolean ascii,
                       gboolean in_include_colon);

/** Append a string representation of a hexdump of the buffer in the
 * ustring.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_hexdumpa (ustring *in_ustr, const guint8 *buffer,
                        int start, int stop, gboolean ascii,
                        gboolean in_include_colon);

/** Create a string representation of a hexdump of the buffer in the
 * ustring.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_hexstream (ustring *in_ustr, const guint8 *buffer,
                         int start, int stop, char in_sep);

/** Append a string representation of a hexstream of the buffer in the
 * ustring.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_hexstreama (ustring *in_ustr, const guint8 *buffer,
                          int start, int stop, char in_sep);

/** Store into the ustring the specified string in the specified type.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_strnset_type(ustring *in_ustr, UStringType type,
                           const char *buffer, int buffer_len);

/** Store string s in ustring.
 *
 * Clears out any other data.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_strset (ustring *in_ustr, const char *s);

/** Store string s in ustring
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_strnset (ustring *in_ustr, const char *s, int n);

/** append string str to ustr, ustr must already contain a valid string
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_strcat (ustring *in_ustr, const char *str);

/** append string str to ustr, ustr must already contain a valid string
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_strncat (ustring *in_ustr, const char *str, int len);

/** append char ch to ustring.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_charcat (ustring *in_ustr, int ch);

/** prepend string str to ustring.
 *
 * @ingroup ustring_util_group
 */
gboolean ustr_strnprepend (ustring *in_ustr, const char *str, int len);

void ustr_debug_print (const ustring *in_ustr);

/** Get the number of currently allocated ustring objects.
 *
 * @internal for testing purposes
 *
 * @ingroup ustring_util_group
 */
int ustr_allocated_count(void);

G_END_DECLS

#endif // RREGADMIN_UTIL_USTRING_H
