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

#include <string.h>

#include <rregadmin/util/path_info.h>
#include <rregadmin/util/malloc.h>

struct rra_path_info_
{
    rra_path_mode mode;
};

rra_path_info*
rra_path_info_new(rra_path_mode in_mode)
{
    rra_path_info *ret_val = rra_new_type(rra_path_info);
    ret_val->mode = in_mode;
    return ret_val;
}

gboolean
rra_path_info_free(rra_path_info *in_pi)
{
    if (in_pi == NULL)
    {
        return FALSE;
    }

    rra_free_type(rra_path_info, in_pi);
    return TRUE;
}

const char*
rra_path_info_get_delim(const rra_path_info *in_pi)
{
    if (rra_path_info_has_option(in_pi, RRA_PATH_OPT_OUT_WIN_DELIMITER))
    {
        return "\\";
    }
    else
    {
        return "/";
    }
}

gboolean
rra_path_info_is_delim(const rra_path_info *in_pi, const char *in_str)
{
    if (rra_path_info_has_option(in_pi, RRA_PATH_OPT_IN_UNIX_DELIMITER)
        && in_str[0] == '/')
    {
        return TRUE;
    }

    if (rra_path_info_has_option(in_pi, RRA_PATH_OPT_IN_WIN_DELIMITER)
        && in_str[0] == '\\')
    {
        return TRUE;
    }

    return FALSE;
}

gboolean
rra_path_info_contains_delim(const rra_path_info *in_pi, const char *in_str)
{
    if (rra_path_info_has_option(in_pi, RRA_PATH_OPT_IN_UNIX_DELIMITER)
        && strstr(in_str, "/") != NULL)
    {
        return TRUE;
    }
    if (rra_path_info_has_option(in_pi, RRA_PATH_OPT_IN_WIN_DELIMITER)
        && strstr(in_str, "\\") != NULL)
    {
        return TRUE;
    }
    return FALSE;
}

rra_path_mode
rra_path_info_get_mode(const rra_path_info *in_pi)
{
    if (in_pi == NULL)
    {
        return 0;
    }

    return in_pi->mode;
}

gboolean
rra_path_info_set_mode(rra_path_info *in_pi, rra_path_mode in_mode)
{
    if (in_pi == NULL)
    {
        return FALSE;
    }

    in_pi->mode = in_mode;
    return TRUE;
}

gboolean
rra_path_info_has_option(const rra_path_info *in_pi, rra_path_mode in_mode)
{
    if (in_pi == NULL)
    {
        return FALSE;
    }

    return (in_pi->mode & in_mode) == in_mode;
}

gboolean
rra_path_info_set_option(rra_path_info *in_pi, rra_path_mode in_mode)
{
    if (in_pi == NULL)
    {
        return FALSE;
    }

    in_pi->mode |= in_mode;
    return TRUE;
}

gboolean
rra_path_info_unset_option(rra_path_info *in_pi, rra_path_mode in_mode)
{
    if (in_pi == NULL)
    {
        return FALSE;
    }

    in_pi->mode &= ~in_mode;
    return TRUE;
}
