/*
 * Authors:         James LewisMoss <jlm@racemi.com>
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

#ifndef RREGADMIN_UTIL_PATH_PART_H
#define RREGADMIN_UTIL_PATH_PART_H 1

#include <rregadmin/util/ustring.h>
#include <rregadmin/util/path_info.h>

#include <rregadmin/util/macros.h>

G_BEGIN_DECLS

/**
 * @defgroup path_part_util_group Path Part
 *
 * @ingroup util_group
 */

/** Types of path parts.
 *
 * @ingroup path_part_util_group
 */
typedef enum
{
    /** Empty.
     */
    RRA_PATH_PART_EMPTY = 0,
    /** Delimeter token id.
     */
    RRA_PATH_PART_DELIM = 1,
    /** Parent token id (i.e. "..")
     */
    RRA_PATH_PART_PARENT = 2,
    /** Current token id (i.e. ".")
     */
    RRA_PATH_PART_CURRENT = 3,
    /** Path element token id.
     */
    RRA_PATH_PART_ELEMENT = 4,
    /** End of tokens id.
     */
    RRA_PATH_PART_END = 5,

    /** Error token id.
     */
    RRA_PATH_PART_ERROR = 10,

} rra_path_part_type;

/** Path Part object.
 *
 * @ingroup path_part_util_group
 */
typedef struct rra_path_part_ rra_path_part;

/** Create a new path part.
 *
 * @ingroup path_part_util_group
 */
rra_path_part* rra_path_part_new(const rra_path_info *in_pi);

/** Create a new path element part.
 *
 * @ingroup path_part_util_group
 */
rra_path_part* rra_path_part_new_element(const rra_path_info *in_pi,
                                         const char *in_part);

/** Copy a path part.
 *
 * @ingroup path_part_util_group
 */
rra_path_part* rra_path_part_copy(const rra_path_part *in_pp);

/** Free a path part.
 *
 * @ingroup path_part_util_group
 */
gboolean rra_path_part_free(rra_path_part *in_pp);

/** Reset the path to base state.
 *
 * @ingroup path_part_util_group
 */
gboolean rra_path_part_clear(rra_path_part *in_pp);

/** Compare two path parts.
 *
 * @ingroup path_part_util_group
 */
int rra_path_part_compare(rra_path_part *in_pp1, rra_path_part *in_pp2);

/** Grab the next token from the string.
 *
 * Leaves the string pointer right past last character used.
 *
 * If it is called twice on the same path_part then the part is reset
 * for the new info.
 *
 * @ingroup path_part_util_group
 */
gboolean rra_path_part_grab_token(rra_path_part *in_pp, const char **in_str);

/** Get the type.
 *
 * @ingroup path_part_util_group
 */
rra_path_part_type rra_path_part_get_type(const rra_path_part *in_pp);

/** Get the value.
 *
 * @ingroup path_part_util_group
 */
const ustring* rra_path_part_get_value(const rra_path_part *in_pp);

G_END_DECLS

#endif // RREGADMIN_UTIL_PATH_PART_H
