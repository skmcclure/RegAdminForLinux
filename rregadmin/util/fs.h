/*
 * Authors:	James Lewismoss <jlm@racemi.com>
 *
 * Copyright (c) 2007 Racemi
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

#ifndef RREGADMIN_UTIL_FS_H
#define RREGADMIN_UTIL_FS_H

#include <glib.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/util/ustring.h>

G_BEGIN_DECLS

/**
 * @defgroup fs_util_group Filesystem Handling Routines
 *
 * @ingroup util_group
 */

/** Find a file pseudo-case-insensitively.
 *
 * The algorithm for finding a file is:
 * 1. If a file matches exactly we've found it.
 * 2. Start with the first segment of the filename and look for it
 *   a. as exact match
 *   b. case insensitively
 *      If multiple entries match case insensitively then the function
 *      returns FALSE.
 * 3. move forward through the filename segments recursively following the
 *    algorithm in step 2.
 *
 * For example:
 * Given the path /Bar/baZ look for the following files:
 * - /Bar/baZ
 * - /bar
 * - /BAR
 * - /[bB][aA][rR]
 * Assuming we found /BAR
 * - /BAR/baZ
 * - /BAR/baz
 * - /BAR/BAZ
 * - /BAR/[bB][aA][zZ]
 *
 * @return whether function succeeded.
 * @retval TRUE if the path is found and changes the string in in_path
 * @retval FALSE if the file is not found and leaves in_path in an
 *               undefined state
 *
 * @ingroup fs_util_group
 */
gboolean find_file_casei_bang(ustring *in_path);

/** Same as find_file_casei_bang, but returns a new string with the modified
 * file path or NULL if the path is not found.
 *
 * @ingroup fs_util_group
 */
ustring* find_file_casei(const ustring *in_path);

gboolean filesysob_exists(const ustring *in_path);

gboolean filesysob_exists_and_non_zero_sized(const ustring *in_path);

/** Create a temporary directory.
 *
 * @param in_dir_prefix for temporary directory
 * @return Newly allocated string with temporary directory name in it.
 */
gchar* rra_mkdtemp(const char *in_dir_prefix);

G_END_DECLS

#endif /* RREGADMIN_UTIL_FS_H */
