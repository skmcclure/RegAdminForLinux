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

#ifndef RREGADMIN_UTIL_CONVERSION_UTILS_H
#define RREGADMIN_UTIL_CONVERSION_UTILS_H 1

#include <glib/gtypes.h>
#include <glib/garray.h>

#include <rregadmin/util/macros.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/value_type.h>

G_BEGIN_DECLS

/** Do a fuzzy conversion from a string to a dword (guint32).
 *
 * @param in_str the string to convert.  It must contain only leading and
 *               trailing whitespace and the digits for the number.
 * @param out_val Where the value parsed is placed.  If an error occurs
 *                this variable will remain unchanged.  This MUST
 *                point to a valid location.
 * @return whether the function succeeded.
 *
 * @ingroup util_group
 */
gboolean fuzzy_str_to_dword(const char *in_str, dword_type *out_val);

/** Do a fuzzy conversion from a string to a qword (gint64).
 *
 * @param in_str the string to convert.  It must contain only leading and
 *               trailing whitespace and the digits for the number.
 * @param out_val Where the value parsed is placed.  If an error occurs
 *                this variable will remain unchanged.  This MUST
 *                point to a valid location.
 * @return whether the function succeeded.
 *
 * @ingroup util_group
 */
gboolean fuzzy_str_to_qword(const char *in_str, qword_type *out_val);

/** Converts strings either like "0xbinary data" or "HEX(binary
 * data)" into binary data.
 *
 * Whitespace is allowed throughout the string and is ignored  except
 * inside the tags "0x" and "HEX".
 *
 * binary data is an arbitrary number of pairs of hex digits
 * ("[0-9a-fA-F]{2}").
 *
 * If there is an odd number of characters then the last character will
 * be interpreted as if it had a 0 in front of it.
 *
 * out_val may contain data already and any new data from in_str is
 * appended to it.  If an error occurs in the middle of the string out_val
 * will likely already have been changed so don't trust out_val after
 * a FALSE return.
 *
 * @ingroup util_group
 */
gboolean str_to_binary(const char *in_str, GByteArray *out_val);

gboolean check_min_max(gint64 in_val, gint64 in_min, gint64 in_max);
gboolean check_convert_results(const char *in_str, char *endptr);

G_END_DECLS

#endif // RREGADMIN_UTIL_CONVERSION_UTILS_H
