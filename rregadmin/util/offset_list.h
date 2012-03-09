/*
 * Authors:	James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi
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

#ifndef RREGADMIN_UTIL_OFFSET_LIST_H
#define RREGADMIN_UTIL_OFFSET_LIST_H 1

#include <rregadmin/util/macros.h>
#include <rregadmin/util/offsets.h>

/**
 * @defgroup offset_list_object List of offsets
 *
 * @ingroup util_group
 */
G_BEGIN_DECLS

typedef struct offset_list_ offset_list;


/** Create a new offset_list using the provided offset_holder.
 *
 * @param in_oh the offset holder the list is associated with
 * @return a new offset_list
 *
 * @ingroup offset_list_object
 */
offset_list* ofslist_new(offset_holder *in_oh);

/** Free an offset_list
 *
 * @param in_ol the offset_list to be freed
 * @return      whether the free succeeded
 *
 * @ingroup offset_list_object
 */
gboolean ofslist_free(offset_list *in_ol);

/** Get the number of offsets in the list
 *
 * @ingroup offset_list_object
 */
gint ofslist_size(const offset_list *in_ol);

/** Get the offset_holder associated with this list.
 *
 * @ingroup offset_list_object
 */
offset_holder* ofslist_get_holder(offset_list *in_ol);

/** Add an offset to the offset_list.
 *
 * Offset must come from the same holder used to create the list.
 *
 * @ingroup offset_list_object
 */
gboolean ofslist_add(offset_list *in_ol, offset in_ofs);

/** Check if the offset is in the offset_list.
 *
 * Offset must come from the same holder used to create the list.
 *
 * @ingroup offset_list_object
 */
gboolean ofslist_contains(offset_list *in_ol, offset in_ofs);

/** Remove the offset from the offset_list.
 *
 * Offset must come from the same holder used to create the list.
 *
 * @ingroup offset_list_object
 */
gboolean ofslist_remove(offset_list *in_ol, offset in_ofs);


/** Get the number of allocated offset lists
 *
 * @internal
 *
 * @ingroup offset_list_object
 */
int ofslist_allocated_count(void);

G_END_DECLS

#endif // RREGADMIN_UTIL_OFFSET_LIST_H
