/*
 * Authors:       James LewisMoss <jlm@racemi.com>
 *
 * Copyright 2006,2007,2008 Racemi Inc
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

#ifndef RREGADMIN_UTIL_PATH_H
#define RREGADMIN_UTIL_PATH_H 1

#include <rregadmin/util/ustring.h>
#include <rregadmin/util/path_info.h>
#include <rregadmin/util/macros.h>

G_BEGIN_DECLS

/**
 * @defgroup path_util_group Path Object
 *
 * @ingroup util_group
 */
typedef struct rra_path_ rra_path;

/** Create a new path.
 *
 * It parses in_str as the path. mode contains options as defined above
 * in RRA_PATH_OPT_*.
 *
 * @ingroup path_util_group
 */
rra_path* rra_path_new_full(const char *in_str, rra_path_mode mode);

/** Create a new path with RRA_PATH_OPT_WIN options.
 *
 * @ingroup path_util_group
 */
rra_path* rra_path_new_win(const char *in_str);

/** Create a new path with RRA_PATH_OPT_UNIX options.
 *
 * @ingroup path_util_group
 */
rra_path* rra_path_new_unix(const char *in_str);

/** Copy a path.
 *
 * @ingroup path_util_group
 */
rra_path* rra_path_copy(const rra_path *in_path);

/** Copy a path resetting the mode bits using mode.
 *
 * @ingroup path_util_group
 */
rra_path* rra_path_copy_full(const rra_path *in_path, rra_path_mode mode);

/** Set the destination path to be the same as the source path
 *
 * @ingroup path_util_group
 */
gboolean rra_path_set(rra_path *in_dest, const rra_path *in_src);

/** Reset the path to the empty state.
 *
 * @ingroup path_util_group
 */
gboolean rra_path_clear(rra_path *in_path);

/** Free a path.
 *
 * @ingroup path_util_group
 */
gboolean rra_path_free(rra_path *in_path);

/** Check the validity of a path.
 *
 * @ingroup path_util_group
 */
gboolean rra_path_is_valid(const rra_path *in_path);

/** Parse a new string into the path.
 *
 * Any data in the path at the time is cleared.
 *
 * @ingroup path_util_group
 */
gboolean rra_path_parse(rra_path *in_path, const char *in_str);

/** Append path in_add to the end of in_dest.
 *
 * \note This modifies the path in place.
 *
 * @ingroup path_util_group
 */
gboolean rra_path_add(rra_path *in_dest, const rra_path *in_add);

/** Append a single part to the path.
 *
 * in_part may not contain path separators (/ or \\).
 *
 * \note This modifies the path in place.
 *
 * @ingroup path_util_group
 */
gboolean rra_path_append_part(rra_path *in_dest, const char *in_part);

/** Prepend a single part to the path.
 *
 * in_part may not contain path separators (/ or \\), but as a special
 * case if you prepend a string containing just a path separator then
 * the path becomes absolute.
 *
 * Absolute paths may not be prepended to.
 *
 * \note This modifies the path in place.
 *
 * @ingroup path_util_group
 */
gboolean rra_path_prepend_part(rra_path *in_dest, const char *in_part);

/** Normalize the path removing all the "." parts and removing all the ".."
 * parts and their parents.
 *
 * ".." handling is complex.  If the path is absolute then any ".."s at the
 * begining of the path are thrown away.  If the path is relative then the
 * ".."s are not thrown away.
 *
 * \note This modifies the path in place.
 *
 * @ingroup path_util_group
 */
gboolean rra_path_normalize(rra_path *in_path);

/** Get the number of parts in the path.
 *
 * @ingroup path_util_group
 */
int rra_path_number_of_parts(const rra_path *in_path);

/** Get the string representation for a particular part.
 *
 * @ingroup path_util_group
 */
const ustring *rra_path_get_part(const rra_path *in_path, guint in_index);

/** Check for a setting.
 *
 * @ingroup path_util_group
 */
gboolean rra_path_has_mode(const rra_path *in_path, rra_path_mode in_mode);

/** Get mode.
 *
 * @ingroup path_util_group
 */
rra_path_mode rra_path_get_mode(const rra_path *in_path);

/** Is the path absolute?
 *
 * @ingroup path_util_group
 */
gboolean rra_path_is_absolute(const rra_path *in_path);

/** Is the path relative?
 *
 * @ingroup path_util_group
 */
gboolean rra_path_is_relative(const rra_path *in_path);

/** Get the dirname part of a path.
 *
 * See dirname() in the std c library.
 *
 * @ingroup path_util_group
 */
const char* rra_path_dirname(const rra_path *in_path);

/** Get the basename part of a path.
 *
 * See basename() in the std c library.
 *
 * @ingroup path_util_group
 */
const char* rra_path_basename(const rra_path *in_path);

/** Get the string representation of the path.
 *
 * RRA_PATH_OPT_WIN_DELIMITER determines whether / or \ is used as the part
 * separator.
 *
 * @ingroup path_util_group
 */
const char* rra_path_as_str(const rra_path *in_path);

/** Get the ustring representation of the path.
 *
 * RRA_PATH_OPT_WIN_DELIMITER determines whether / or \ is used as the part
 * separator.
 *
 * @ingroup path_util_group
 */
const ustring* rra_path_as_ustr(const rra_path *in_path);

/** Get the delimiter that will/is used to separate parts.
 *
 * @ingroup path_util_group
 */
const char* rra_path_get_delimiter(const rra_path *in_path);

/** Compare paths like strcmp.
 *
 * Absolute paths are greater than non-absolute paths.
 * Longer paths (assuming all else is equal) are greater than shorter paths.
 *
 * @ingroup path_util_group
 */
gint rra_path_compare(const rra_path *in_path1, const rra_path *in_path2);

/** Test for path equality.
 *
 * @see rra_path_compare
 *
 * @ingroup path_util_group
 */
gboolean rra_path_equal(const rra_path *in_path1, const rra_path *in_path2);

/** Test if one path is less than another.
 *
 * @see rra_path_compare
 *
 * @ingroup path_util_group
 */
gboolean rra_path_less(const rra_path *in_path1, const rra_path *in_path2);

/** Determines whether in_path1 is a subpath in in_path2.
 *
 * For example:
 * /foo is a subpath of /foo/bar
 * /foo/bar is not a subpath of /foo/baz
 *
 * /foo is a subpath of /foo.
 *
 * Both paths must be either absolute or relative. One can't be relative
 * and the other absolute.
 *
 * @ingroup path_util_group
 */
gboolean rra_path_is_subpath(const rra_path *in_path1,
                             const rra_path *in_path2);

/** Returns a new rra_path object containing only the subpath from in_path2.
 *
 * See rra_path_is_subpath() for an explanation of subpaths.
 *
 * @ingroup path_util_group
 */
rra_path* rra_path_make_subpath(const rra_path *in_path1,
                              const rra_path *in_path2);

/** Trim the number of parts off the front of the path.
 *
 * If the path is absolute then the first of the trim parts is making the path
 * relative.
 *
 * For example rra_path_trim_front("/foo/bar", 1) => "foo/bar"
 *
 * If you try to trim more than the available parts then the path remains
 * unchanged and FALSE is returned.
 *
 * \note This modifies the path in place.
 *
 * @ingroup path_util_group
 */
gboolean rra_path_trim_front(rra_path *in_path, guint in_parts);

/** Trim the number of parts off the back of the path.
 *
 * If the path is absolute then the last of the trim parts is making the path
 * relative.
 *
 * For example rra_path_trim_back("/foo/bar", 1) => "/foo"
 *             rra_path_trim_back("/foo", 1)     => "/"
 *             rra_path_trim_back("/", 1)        => ""
 *
 * If you try to trim more than the available parts then the path remains
 * unchanged and FALSE is returned.
 *
 * \note This modifies the path in place.
 *
 * @ingroup path_util_group
 */
gboolean rra_path_trim_back(rra_path *in_path, guint in_parts);

/** Get count of allocated path objects.
 *
 * @internal for testing purposes
 *
 * @ingroup path_util_group
 */
int rra_path_allocated_count(void);

G_END_DECLS

#endif // RREGADMIN_UTIL_PATH_H
