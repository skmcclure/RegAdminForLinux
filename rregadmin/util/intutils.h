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

#ifndef RREGADMIN_UTIL_INTUTILS_H
#define RREGADMIN_UTIL_INTUTILS_H 1

#include <glib/gtypes.h>
#include <glib/garray.h>

#include <rregadmin/util/macros.h>
#include <rregadmin/util/ustring.h>

G_BEGIN_DECLS

/** Convert a string into a signed 8 bit integer.
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
gboolean str_to_gint8(const char *in_str, gint8 *out_val);

/** Convert a string into a signed 16 bit integer.
 *
 * @see str_to_gint8
 *
 * @ingroup util_group
 */
gboolean str_to_gint16(const char *in_str, gint16 *out_val);

/** Convert a string into a signed 32 bit integer.
 *
 * @see str_to_gint8
 *
 * @ingroup util_group
 */
gboolean str_to_gint32(const char *in_str, gint32 *out_val);

/** Convert a string into a signed 64 bit integer.
 *
 * @see str_to_gint8
 *
 * @ingroup util_group
 */
gboolean str_to_gint64(const char *in_str, gint64 *out_val);

/** Convert a string into a unsigned 8 bit integer.
 *
 * @see str_to_gint8
 *
 * @ingroup util_group
 */
gboolean str_to_guint8(const char *in_str, guint8 *out_val);

/** Convert a string into a unsigned 16 bit integer.
 *
 * @see str_to_gint8
 *
 * @ingroup util_group
 */
gboolean str_to_guint16(const char *in_str, guint16 *out_val);

/** Convert a string into a unsigned 32 bit integer.
 *
 * @see str_to_gint8
 *
 * @ingroup util_group
 */
gboolean str_to_guint32(const char *in_str, guint32 *out_val);

/** Convert a string into a unsigned 64 bit integer.
 *
 * @see str_to_gint8
 *
 * @ingroup util_group
 */
gboolean str_to_guint64(const char *in_str, guint64 *out_val);

G_END_DECLS

#endif // RREGADMIN_UTIL_INTUTILS_H
