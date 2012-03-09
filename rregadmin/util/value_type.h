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

#ifndef RREGADMIN_UTIL_VALUE_TYPES_H
#define RREGADMIN_UTIL_VALUE_TYPES_H 1

#include <stdio.h>
#include <glib/gtypes.h>

#include <rregadmin/util/macros.h>
#include <rregadmin/util/ustring.h>

/**
 * @defgroup value_type_util_group value type handling code
 *
 * @ingroup util_group
 */

G_BEGIN_DECLS

/** Windows type.
 *
 * @ingroup value_type_util_group
 */
typedef guint32 dword_type;

/** Windows type.
 *
 * @ingroup value_type_util_group
 */
typedef gint64 qword_type;

/** Types enum.
 *
 * @ingroup value_type_util_group
 */
typedef enum
{
    /** no type
     */
    REG_NONE = 0,

    /** string type.
     *
     * @note Type: ustring*
     */
    REG_SZ,

    /** string, includes %ENVVAR% (expanded by caller).
     *
     * @note Type: ustring*
     */
    REG_EXPAND_SZ,

    /** binary format, callerspecific
     *
     * @note Type: GByteArray*
     */
    REG_BINARY,

    /** DWORD in little endian format
     *
     * YES, REG_DWORD == REG_DWORD_LITTLE_ENDIAN
     *
     * @note Type: dword_type
     */
    REG_DWORD,

    /** DWORD in little endian format
     *
     * @note Type: dword_type
     *
     * @see REG_DWORD
     */
    REG_DWORD_LITTLE_ENDIAN = REG_DWORD,

    /** DWORD in big endian format
     *
     * @note Type: dword_type
     */
    REG_DWORD_BIG_ENDIAN,

    /** symbolic link (UNICODE)
     *
     * I've never seen one of these.
     *
     * @note Type: ustring*
     */
    REG_LINK,

    /** multiple strings, delimited by \\0, terminated by \\0\\0 (ASCII)
     *
     * @note Type: ustring*
     */
    REG_MULTI_SZ,

    /** resource list? huh?
     *
     * @note Type: GByteArray*
     */
    REG_RESOURCE_LIST,

    /** full resource descriptor? huh?
     *
     * @note Type: GByteArray*
     */
    REG_FULL_RESOURCE_DESCRIPTOR,

    /** Don't know what this is.
     *
     * @note Type: GByteArray*
     */
    REG_RESOURCE_REQUIREMENTS_LIST,

    /** 8 byte integer in little endian format
     *
     * @note Type: qword_type
     */
    REG_QWORD,

    /** 8 byte integer in little endian format
     *
     * @note Type: qword_type
     *
     * @see REG_QWORD
     */
    REG_QWORD_LITTLE_ENDIAN = REG_QWORD,

    /** The maximum allowed type.
     */
    REG_MAX = REG_QWORD_LITTLE_ENDIAN,

    /** Empty type.
     */
    REG_EMPTY,

    /** Auto type.
     */
    REG_AUTO,

} ValueType;

/** Parse a string into a type.
 *
 * This handles both REG_SZ and SZ forms case insensitively.
 *
 * @ingroup value_type_util_group
 */
ValueType value_type_from_string (const char *tstr);

/** Returns short string forms for types.
 *
 * so REG_SZ -> "SZ"
 *
 * @ingroup value_type_util_group
 */
const char *value_type_get_string (ValueType type);

/** Returns a long description of the type.
 *
 * @ingroup value_type_util_group
 */
const char *value_type_get_description (ValueType type);

/** Check to see if a type value is within the allowed range.
 *
 * @ingroup value_type_util_group
 */
gboolean value_type_is_valid(ValueType type);

/** Check to see if a type value is a string type.
 *
 * @ingroup value_type_util_group
 */
gboolean value_type_is_string(ValueType type);

/** Check to see if a type value is a binary type.
 *
 * @ingroup value_type_util_group
 */
gboolean value_type_is_binary(ValueType type);

/** Check to see if a type value is an integral type.
 *
 * @ingroup value_type_util_group
 */
gboolean value_type_is_integral(ValueType type);


G_END_DECLS

#endif // RREGADMIN_UTIL_VALUE_TYPES_H
