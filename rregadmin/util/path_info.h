/*
 * Authors:       James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006,2007 Racemi Inc
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

#ifndef RREGADMIN_UTIL_PATH_INFO_H
#define RREGADMIN_UTIL_PATH_INFO_H 1

#include <glib.h>

#include <rregadmin/util/macros.h>

G_BEGIN_DECLS

/**
 * @defgroup path_info_util_group Path Info Object
 *
 * @ingroup util_group
 */
typedef struct rra_path_info_ rra_path_info;

typedef guint32 rra_path_mode;

/** Options for path objects.
 *
 * @ingroup path_info_util_group
 */
typedef enum
{
    /** Tag indicating paths should have xml escapes handled.
     */
    RRA_PATH_OPT_IN_XML_ESCAPES = 1 << 4,

    /** Tag indicating paths should have xml escapes used on output.
     */
    RRA_PATH_OPT_OUT_XML_ESCAPES = 1 << 5,

    /** Tag indicating paths should have a minimal set of xml escapes
     * used on output.
     */
    RRA_PATH_OPT_OUT_MINIMAL_XML_ESCAPES = 1 << 6,

    /** Tag indicating that the str rep for the path should use
     * windows delimiters.
     */
    RRA_PATH_OPT_IN_WIN_DELIMITER = 1 << 8,

    /** Tag indicating that the str rep for the path should use
     * unix delimiters.
     */
    RRA_PATH_OPT_IN_UNIX_DELIMITER = 1 << 9,

    /** Tag indicating only windows delimiters (\\) should be used.
     */
    RRA_PATH_OPT_OUT_WIN_DELIMITER = 1 << 12,

    /** Tag indicating that comparisons of paths should be case
     * insensitive
     */
    RRA_PATH_OPT_CASE_INSENSITIVE = 1 << 24,

    /** Grouping tag for windows options.
     */
    RRA_PATH_OPT_WIN = (RRA_PATH_OPT_OUT_WIN_DELIMITER
                        | RRA_PATH_OPT_CASE_INSENSITIVE
                        | RRA_PATH_OPT_IN_WIN_DELIMITER),

    /** Grouping tag for unix options.
     */
    RRA_PATH_OPT_UNIX = (RRA_PATH_OPT_IN_XML_ESCAPES
                         | RRA_PATH_OPT_OUT_MINIMAL_XML_ESCAPES
                         | RRA_PATH_OPT_IN_UNIX_DELIMITER),

    /** Grouping tag for mixture of unix and win options.
     */
    RRA_PATH_OPT_MIX = (RRA_PATH_OPT_OUT_WIN_DELIMITER
                        | RRA_PATH_OPT_CASE_INSENSITIVE
                        | RRA_PATH_OPT_IN_WIN_DELIMITER
                        | RRA_PATH_OPT_IN_XML_ESCAPES
                        | RRA_PATH_OPT_OUT_MINIMAL_XML_ESCAPES
                        | RRA_PATH_OPT_IN_UNIX_DELIMITER),

}  RRA_PATH_MODE;

/** Create a new path_info.
 *
 * @ingroup path_info_util_group
 */
rra_path_info* rra_path_info_new(rra_path_mode in_mode);

/** Free a path_info.
 *
 * @ingroup path_info_util_group
 */
gboolean rra_path_info_free(rra_path_info *in_pi);

/** Get the delimiter that will/is used to separate parts.
 *
 * @ingroup path_info_util_group
 */
const char* rra_path_info_get_delim(const rra_path_info *in_pi);

/** Check to see if a valid delimiter is at the begining of the string.
 *
 * @ingroup path_info_util_group
 */
gboolean rra_path_info_is_delim(const rra_path_info *in_pi,
                                const char *in_str);

/** Check to see if the string contains a delimiter.
 *
 * @ingroup path_info_util_group
 */
gboolean rra_path_info_contains_delim(const rra_path_info *in_pi,
                                      const char *in_str);

/** Get the mode mask.
 *
 * @ingroup path_info_util_group
 */
rra_path_mode rra_path_info_get_mode(const rra_path_info *in_pi);

/** Set the mode mask.
 *
 * @ingroup path_info_util_group
 */
gboolean rra_path_info_set_mode(rra_path_info *in_pi, rra_path_mode in_mode);

/** Check for an option.
 *
 * @ingroup path_info_util_group
 */
gboolean rra_path_info_has_option(const rra_path_info *in_pi,
                                  rra_path_mode in_mode);

/** Set an option.
 *
 * @ingroup path_info_util_group
 */
gboolean rra_path_info_set_option(rra_path_info *in_pi,
                                  rra_path_mode in_mode);

/** Unset an option.
 *
 * @ingroup path_info_util_group
 */
gboolean rra_path_info_unset_option(rra_path_info *in_pi,
                                    rra_path_mode in_mode);

G_END_DECLS

#endif // RREGADMIN_UTIL_PATH_INFO_H
