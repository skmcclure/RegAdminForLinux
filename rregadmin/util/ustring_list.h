/*
 * Authors:         James LewisMoss <jlm@racemi.com>
 *
 * Copyright 2006,2008 Racemi Inc
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

#ifndef RREGADMIN_UTIL_USTRING_LIST_H
#define RREGADMIN_UTIL_USTRING_LIST_H 1

#include <stdarg.h>

#include <rregadmin/util/macros.h>
#include <rregadmin/util/ustring.h>

G_BEGIN_DECLS

/**
 * @defgroup ustring_list_util_group UString List Object
 *
 * @ingroup util_group
 */

typedef struct ustring_list_ ustring_list;

/** Create an empty ustring_list.
 *
 * @ingroup ustring_list_util_group
 */
ustring_list* ustrlist_new(void) G_GNUC_MALLOC;

/** Create an ustring_list with the ustrings passed in as args.
 *
 * Terminate the list with NULL.
 *
 * @ingroup ustring_list_util_group
 */
ustring_list* ustrlist_new_list(ustring *in_str, ...)
    G_GNUC_MALLOC G_GNUC_NULL_TERMINATED;

/** Create a copy of a ustring list.
 *
 * @ingroup ustring_list_util_group
 */
ustring_list* ustrlist_copy (const ustring_list *in_ul)
    G_GNUC_MALLOC;

/** Check to see if this is a valid ustring list
 *
 * @ingroup ustring_list_util_group
 */
gboolean ustrlist_is_valid (const ustring_list *in_ul) G_GNUC_PURE;

/** Free a dynamically created ustring_list
 *
 * @ingroup ustring_list_util_group
 */
gboolean ustrlist_free (ustring_list *in_ul);

/** Clear the ustrings from the ustring_list.
 *
 * @ingroup ustring_list_util_group
 */
gboolean ustrlist_clear (ustring_list *in_ul);

/** Get the number of ustrings in the list.
 *
 * @ingroup ustring_list_util_group
 */
int ustrlist_size (const ustring_list *in_ul) G_GNUC_PURE;

/** Get the ustring at index.
 *
 * \bug This doesn't actually do this now.
 *
 * @ingroup ustring_list_util_group
 */
const ustring* ustrlist_get(const ustring_list *in_ul, int in_index)
    G_GNUC_PURE;

/** Append a ustring to the list.
 *
 * @ingroup ustring_list_util_group
 */
gboolean ustrlist_append(ustring_list *in_ul, ustring *in_str);

/** Check to see if two ustring_lists are equal.
 *
 * @ingroup ustring_list_util_group
 */
gboolean ustrlist_equal(const ustring_list *in_ul1,
                        const ustring_list *in_ul2);

/** Compare two ustring_lists.
 *
 * @ingroup ustring_list_util_group
 */
int ustrlist_compare(const ustring_list *in_ul1,
                     const ustring_list *in_ul2);

/** Get the number of currently allocated ustring objects.
 *
 * @internal for testing purposes
 *
 * @ingroup ustring_list_util_group
 */
int ustrlist_allocated_count(void);

/** Print debug output for ustring_list.
 *
 * @internal
 *
 * @ingroup ustring_list_util_group
 */
void ustrlist_debug_print(const ustring_list *in_ul);

G_END_DECLS

#endif // RREGADMIN_UTIL_USTRING_LIST_H
