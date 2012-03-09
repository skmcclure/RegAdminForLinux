/*
 *  Authors: James LewisMoss <jlm@racemi.com> or <jim@lewismoss.org>
 *
 *  Copyright 2008 Racemi Inc
 *
 *  This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef RREGADMIN_UTIL_FLAG_MAP_H
#define RREGADMIN_UTIL_FLAG_MAP_H

#include <rregadmin/util/macros.h>
#include <rregadmin/util/ustring.h>

/**
 * @defgroup flag_map_util_group Flag Map
 *
 * @ingroup util_group
 */


G_BEGIN_DECLS

/** Type for flags in FlagMap.
 *
 * @ingroup flag_map_util_group
 */
typedef guint32 fm_flag_type;

struct FlagMap_
{
    const char *name;
    const char *long_name;
    const char *descrip;
    fm_flag_type flag;
};

/** Structure holding information mapping integral flags to names,
 * long names and descriptions.
 *
 * @ingroup flag_map_util_group
 */
typedef struct FlagMap_ FlagMap;

/** Parse a string into a flags variable.
 *
 * When in_end_char or '\0' is encountered in in_str the parsing is complete.
 *
 * If parsing completes without finding in_end_char then FALSE is returned.
 *
 * If in_reset_on_error is TRUE with a FALSE return then in_str and
 * in_flags are set to their original values.
 *
 * If parsing completes successfully then in_str is set to point after the
 * last character parsed.
 *
 * @ingroup flag_map_util_group
 */
gboolean flag_map_names_to_flags(const FlagMap *in_map,
                                 const char **in_str, fm_flag_type *in_flags,
                                 char in_end_char,
                                 gboolean in_reset_on_error);

/** Put strings matching flag names into a string.
 *
 * in_flags is changed to reflect the flags handled.
 *
 * If all the flags aren't matched and in_check_all is TRUE then return
 * FALSE.  If an error occurs in_ustr is reset to before it was modified
 * and in_flags is reset to the original value.
 *
 * @ingroup flag_map_util_group
 */
gboolean flag_map_flags_to_names(const FlagMap *in_map,
                                 fm_flag_type *in_flags, ustring *in_ustr,
                                 gboolean in_check_all);

G_END_DECLS

#endif /* RREGADMIN_UTIL_FLAG_MAP_H */
