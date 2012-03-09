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

#include <rregadmin/config.h>

#include <stdio.h>
#include <string.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/util/path.h>
#include <rregadmin/util/log.h>
#include <rregadmin/util/malloc.h>
#include <rregadmin/util/path_part.h>

#define PATH_MAGIC_NUMBER 0x98077089

/** Structure to contain a path.
 *
 * @internal
 *
 * @ingroup path_util_group
 */
struct rra_path_
{
    /** Magic number for validity check.
     */
    guint32 magic;
    /** The parts of the path.
     */
    GPtrArray *parts;
    /** Whether the path is absolute or not.
     */
    gboolean absolute;
    /** path info struct.
     */
    rra_path_info *pi;
    /** Temp holder for dirname string.
     *
     * mutable.
     */
    ustring *dirname;
    /** Temp holder for str string.
     *
     * mutable
     */
    ustring *str;
};

static rra_path_part* get_part(const rra_path *in_path, guint in_index);

static int allocated_count = 0;

int
rra_path_allocated_count(void)
{
    return allocated_count;
}

rra_path*
rra_path_new_full(const char *in_str, guint32 mode)
{
    rra_path *ret_val = rra_new_type(rra_path);

    ret_val->magic = PATH_MAGIC_NUMBER;
    ret_val->parts = g_ptr_array_new();
    ret_val->pi = rra_path_info_new(mode);
    ret_val->absolute = FALSE;

    allocated_count++;

    if (in_str != NULL
        && in_str[0] != '\0')
    {
        if (!rra_path_parse(ret_val, in_str))
        {
            rra_path_free(ret_val);
            return NULL;
        }
    }

    return ret_val;
}

rra_path*
rra_path_new_win(const char *in_str)
{
    return rra_path_new_full(in_str, RRA_PATH_OPT_WIN);
}

rra_path*
rra_path_new_unix(const char *in_str)
{
    return rra_path_new_full(in_str, RRA_PATH_OPT_UNIX);
}

rra_path*
rra_path_copy(const rra_path *in_path)
{
    rra_path *ret_val =
        rra_path_new_full(0, rra_path_info_get_mode(in_path->pi));

    if (!rra_path_set(ret_val, in_path))
    {
        rra_path_free(ret_val);
        return NULL;
    }

    return ret_val;
}

rra_path*
rra_path_copy_full(const rra_path *in_path, guint32 in_mode)
{
    rra_path *ret_val = rra_path_new_full(rra_path_as_str(in_path), in_mode);

    if (!rra_path_set(ret_val, in_path))
    {
        rra_path_free(ret_val);
        return NULL;
    }

    return ret_val;
}

gboolean
rra_path_set(rra_path *in_dest, const rra_path *in_src)
{
    guint i;

    if (!rra_path_clear(in_dest))
    {
        return FALSE;
    }

    in_dest->absolute = in_src->absolute;

    for (i = 0; i < in_src->parts->len; i++)
    {
        rra_path_part *part = rra_path_part_copy(get_part(in_src, i));
        g_ptr_array_add(in_dest->parts, part);
    }

    return TRUE;
}

static void
rra_path_clear_mutable(rra_path *in_path)
{
    if (in_path->dirname != NULL)
    {
        ustr_free(in_path->dirname);
        in_path->dirname = NULL;
    }

    if (in_path->str != NULL)
    {
        ustr_free(in_path->str);
        in_path->str = NULL;
    }
}

gboolean
rra_path_clear(rra_path *in_path)
{
    if (!rra_path_is_valid(in_path))
    {
        return FALSE;
    }

    in_path->absolute = FALSE;

    if (in_path->parts != NULL)
    {
        while (rra_path_number_of_parts(in_path) > 0)
        {
            rra_path_part *part = get_part(in_path, 0);
            g_ptr_array_remove(in_path->parts, part);
            rra_path_part_free(part);
        }
    }

    rra_path_clear_mutable(in_path);

    return TRUE;
}

gboolean
rra_path_free(rra_path *in_path)
{
    if (!rra_path_clear(in_path))
    {
        return FALSE;
    }

    rra_path_info_free(in_path->pi);

    if (in_path->parts != NULL)
    {
        g_ptr_array_free(in_path->parts, TRUE);
        in_path->parts = NULL;
    }

    in_path->magic = 0;
    rra_free_type(rra_path, in_path);

    allocated_count--;

    return TRUE;
}

gboolean
rra_path_is_valid(const rra_path *in_path)
{
    if (in_path == NULL)
    {
        return FALSE;
    }

    if (in_path->magic != PATH_MAGIC_NUMBER)
    {
        return FALSE;
    }

    if (in_path->parts == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

int
rra_path_number_of_parts(const rra_path *in_path)
{
    return in_path->parts->len;
}

static rra_path_part*
get_part(const rra_path *in_path, guint in_index)
{
    return (rra_path_part*)g_ptr_array_index(in_path->parts, in_index);
}

const ustring*
rra_path_get_part(const rra_path *in_path, guint in_index)
{
    if (in_index >= in_path->parts->len)
    {
        return NULL;
    }
    else
    {
        return rra_path_part_get_value(get_part(in_path, in_index));
    }
}

gboolean
rra_path_is_absolute(const rra_path *in_path)
{
    return in_path->absolute;
}

gboolean
rra_path_is_relative(const rra_path *in_path)
{
    return !in_path->absolute;
}

gboolean
rra_path_has_mode(const rra_path *in_path, guint32 in_mode)
{
    return rra_path_info_has_option(in_path->pi, in_mode);
}

rra_path_mode
rra_path_get_mode(const rra_path *in_path)
{
    return rra_path_info_get_mode(in_path->pi);
}

const ustring*
rra_path_as_ustr(const rra_path *in_path)
{
    if (in_path->str == NULL)
    {
        int i;

        // Think of this as mutable data.
        ((rra_path*)in_path)->str = ustr_new();

        ustr_strset(in_path->str, "");

        if (rra_path_is_absolute(in_path))
        {
            ustr_strcat(in_path->str,
                        rra_path_get_delimiter(in_path));
        }

        for (i = 0; i < rra_path_number_of_parts(in_path); i++)
        {
            ustr_strcat(in_path->str,
                        ustr_as_utf8(rra_path_get_part(in_path, i)));
            if (i != rra_path_number_of_parts(in_path) - 1)
            {
                ustr_strcat(in_path->str,
                            rra_path_get_delimiter(in_path));
            }
        }
    }

    return in_path->str;
}

const char*
rra_path_as_str(const rra_path *in_path)
{
    return ustr_as_utf8(rra_path_as_ustr(in_path));
}

const char*
rra_path_dirname(const rra_path *in_path)
{
    if (rra_path_number_of_parts(in_path) == 0)
    {
        if (rra_path_is_absolute(in_path))
        {
            return "/";
        }
        else
        {
            return ".";
        }
    }
    else if (rra_path_number_of_parts(in_path) == 1)
    {
        if (rra_path_is_absolute(in_path))
        {
            return "/";
        }
        else
        {
            return ".";
        }
    }
    else
    {
        int i;

        if (in_path->dirname == NULL)
        {
            // Think of this as mutable data.
            ((rra_path*)in_path)->dirname = ustr_new();

            if (rra_path_is_absolute(in_path))
            {
                ustr_strcat(in_path->dirname,
                            rra_path_get_delimiter(in_path));
            }

            for (i = 0; i < rra_path_number_of_parts(in_path) - 1; i++)
            {
                ustr_strcat(in_path->dirname,
                            ustr_as_utf8(rra_path_get_part(in_path, i)));
                ustr_strcat(in_path->dirname,
                            rra_path_get_delimiter(in_path));
            }

            // remove trailing delimiter
            ustr_trim_back(in_path->dirname, 1);
        }

        return ustr_as_utf8(in_path->dirname);
    }
}

const char*
rra_path_basename(const rra_path *in_path)
{
    if (rra_path_number_of_parts(in_path) == 0)
    {
        return "";
    }
    else
    {
        return ustr_as_utf8(
            rra_path_get_part(in_path,
                              rra_path_number_of_parts(in_path) - 1));
    }
}

const char*
rra_path_get_delimiter(const rra_path *in_path)
{
    return rra_path_info_get_delim(in_path->pi);
}

gboolean
rra_path_normalize(rra_path *in_path)
{
    guint i;
    // remove all the "." entries
    GPtrArray *new_arr = g_ptr_array_new();
    for (i = 0; i < in_path->parts->len; i++)
    {
        rra_path_part *part = get_part(in_path, i);
        if (rra_path_part_get_type(part) == RRA_PATH_PART_CURRENT)
        {
            /* throw away "." parts. */
            rra_path_part_free(part);
        }
        else if (rra_path_part_get_type(part) == RRA_PATH_PART_PARENT)
        {
            if (new_arr->len == 0)
            {
                if (rra_path_is_absolute(in_path))
                {
                    /* With ".." and an absolute path we can throw it away */
                    rra_path_part_free(part);
                }
                else
                {
                    /* With a relative path we have to keep it. */
                    g_ptr_array_add(new_arr, part);
                }
            }
            else
            {
                rra_path_part *last_new =
                    (rra_path_part*)g_ptr_array_index(new_arr,
                                                      new_arr->len - 1);

                if (rra_path_part_get_type(last_new) == RRA_PATH_PART_PARENT)
                {
                    /* We already have a ".." add another one */
                    g_ptr_array_add(new_arr, part);
                }
                else
                {
                    /* We can pop off one from the stack and throw
                       it and the ".." away */
                    g_ptr_array_remove_index(new_arr, new_arr->len - 1);
                    rra_path_part_free(part);
                    rra_path_part_free(last_new);
                }
            }
        }
        else
        {
            g_ptr_array_add(new_arr, part);
        }
    }
    g_ptr_array_free(in_path->parts, TRUE);
    in_path->parts = new_arr;

    rra_path_clear_mutable(in_path);

    return TRUE;
}

gboolean
rra_path_parse(rra_path *in_path, const char *in_str)
{
    gboolean ret_val = TRUE;
    const char *index = in_str;

    if (!rra_path_clear(in_path))
    {
        return FALSE;
    }

    while(TRUE)
    {
        rra_path_part *part = rra_path_part_new(in_path->pi);
        rra_path_part_grab_token(part, &index);

        if (rra_path_part_get_type(part) == RRA_PATH_PART_END
            || rra_path_part_get_type(part) == RRA_PATH_PART_EMPTY)
        {
            rra_path_part_free(part);
            break;
        }
        else if (rra_path_part_get_type(part) == RRA_PATH_PART_ERROR)
        {
            ret_val = FALSE;
            rra_path_part_free(part);
            break;
        }
        else if (rra_path_part_get_type(part) == RRA_PATH_PART_DELIM)
        {
            /* The first token is a delimeter.  We have an absolute path. */
            if (rra_path_number_of_parts(in_path) == 0)
            {
                in_path->absolute = TRUE;
            }
            rra_path_part_free(part);
        }
        else
        {
           g_ptr_array_add(in_path->parts, part);
        }
    }
    return ret_val;
}

gboolean
rra_path_add(rra_path *in_dest, const rra_path *in_add)
{
    guint i;

    if (rra_path_is_absolute(in_add))
    {
        return FALSE;
    }

    for(i = 0; i < in_add->parts->len; i++)
    {
        rra_path_part *part = rra_path_part_copy(get_part(in_add, i));
        g_ptr_array_add(in_dest->parts, part);
    }

    rra_path_clear_mutable(in_dest);

    return TRUE;
}

gboolean
rra_path_append_part(rra_path *in_dest, const char *in_part)
{
    if (in_dest == NULL || in_part == NULL)
    {
        return FALSE;
    }
    if (strlen(in_part) == 0)
    {
        return FALSE;
    }
    if (rra_path_info_contains_delim(in_dest->pi, in_part))
    {
        return FALSE;
    }

    rra_path_part *part = rra_path_part_new_element(in_dest->pi, in_part);

    g_ptr_array_add(in_dest->parts, part);

    rra_path_clear_mutable(in_dest);

    return TRUE;
}

static void
g_ptr_array_prepend(GPtrArray *in_arr, gpointer in_item)
{
    g_ptr_array_set_size(in_arr, in_arr->len + 1);

    memmove(in_arr->pdata + 1, in_arr->pdata,
            (in_arr->len - 1) * sizeof(gpointer));

    in_arr->pdata[0] = in_item;
}

gboolean
rra_path_prepend_part(rra_path *in_dest, const char *in_part)
{
    if (in_dest == NULL || in_part == NULL)
    {
        return FALSE;
    }
    if (rra_path_is_absolute(in_dest))
    {
        return FALSE;
    }
    if (strlen(in_part) == 0)
    {
        return FALSE;
    }

    if (rra_path_info_contains_delim(in_dest->pi, in_part))
    {
        if (strcmp(in_part, "/") == 0
            || strcmp(in_part, "\\") == 0)
        {
            in_dest->absolute = TRUE;
            rra_path_clear_mutable(in_dest);
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        rra_path_part *part = rra_path_part_new_element(in_dest->pi, in_part);

        g_ptr_array_prepend(in_dest->parts, part);

        rra_path_clear_mutable(in_dest);
        return TRUE;
    }
}

static int
part_compare(const rra_path *in_path1, const rra_path *in_path2,
             int in_part)
{
    return rra_path_part_compare(get_part(in_path1, in_part),
                                 get_part(in_path2, in_part));
}

gint
rra_path_compare(const rra_path *in_path1, const rra_path *in_path2)
{
    gint ret_val = 0;

    int mode = 0;
    if (rra_path_has_mode(in_path1, RRA_PATH_OPT_CASE_INSENSITIVE)
        || rra_path_has_mode(in_path2, RRA_PATH_OPT_CASE_INSENSITIVE))
    {
        mode |= RRA_PATH_OPT_CASE_INSENSITIVE;
    }

    rra_path *p1 = rra_path_copy_full(in_path1, mode);
    rra_path *p2 = rra_path_copy_full(in_path2, mode);

    rra_path_normalize(p1);
    rra_path_normalize(p2);

    if (rra_path_is_absolute(p1) && !rra_path_is_absolute(p2))
    {
        rra_path_free(p1);
        rra_path_free(p2);
        return 1;
    }
    if (!rra_path_is_absolute(p1) && rra_path_is_absolute(p2))
    {
        rra_path_free(p1);
        rra_path_free(p2);
        return -1;
    }

    int parts_count = 0;
    if (rra_path_number_of_parts(p1) > rra_path_number_of_parts(p2))
    {
        ret_val = 1;
        parts_count = rra_path_number_of_parts(p2);
    }
    else if (rra_path_number_of_parts(p1) < rra_path_number_of_parts(p2))
    {
        ret_val = -1;
        parts_count = rra_path_number_of_parts(p1);
    }
    else
    {
        ret_val = 0;
        parts_count = rra_path_number_of_parts(p1);
    }

    int i;
    for (i = 0; i < parts_count; i++)
    {
        int tmp_ret_val = part_compare(p1, p2, i);

        if (tmp_ret_val != 0)
        {
            ret_val = tmp_ret_val;
            break;
        }
    }

    rra_path_free(p1);
    rra_path_free(p2);

    return ret_val;
}

gboolean
rra_path_equal(const rra_path *in_path1, const rra_path *in_path2)
{
    return rra_path_compare(in_path1, in_path2) == 0;
}

gboolean
rra_path_less(const rra_path *in_path1, const rra_path *in_path2)
{
    return rra_path_compare(in_path1, in_path2) < 0;
}

gboolean
rra_path_is_subpath(const rra_path *in_path1, const rra_path *in_path2)
{
    if (rra_path_is_absolute(in_path1) && rra_path_is_relative(in_path2))
    {
        return FALSE;
    }

    if (rra_path_is_relative(in_path1) && rra_path_is_absolute(in_path2))
    {
        return FALSE;
    }

    guint32 mode;
    if (rra_path_info_has_option(in_path1->pi, RRA_PATH_OPT_CASE_INSENSITIVE)
        || rra_path_info_has_option(in_path2->pi,
                                    RRA_PATH_OPT_CASE_INSENSITIVE))
    {
        mode = RRA_PATH_OPT_CASE_INSENSITIVE;
    }

    /* copy the paths so we can normalize them before comparing. */
    rra_path *p1 = rra_path_copy_full(in_path1, mode);
    rra_path *p2 = rra_path_copy_full(in_path2, mode);
    gboolean ret_val = TRUE;

    rra_path_normalize(p1);
    rra_path_normalize(p2);

    if (rra_path_number_of_parts(p1) > rra_path_number_of_parts(p2))
    {
        ret_val = FALSE;
    }
    else
    {
        int i;
        for (i = 0; i < rra_path_number_of_parts(p1); i++)
        {
            if (part_compare(p1, p2, i) != 0)
            {
                ret_val = FALSE;
                break;
            }
        }
    }

    rra_path_free(p1);
    rra_path_free(p2);

    return ret_val;
}

rra_path*
rra_path_make_subpath(const rra_path *in_path1, const rra_path *in_path2)
{
    int i;

    if (!rra_path_is_subpath(in_path1, in_path2))
    {
        return NULL;
    }

    rra_path *ret_val =
        rra_path_new_full("", rra_path_info_get_mode(in_path2->pi));

    for (i = rra_path_number_of_parts(in_path1);
         i < rra_path_number_of_parts(in_path2);
         i++)
    {
        rra_path_part *part = rra_path_part_copy(get_part(in_path2, i));
        g_ptr_array_add(ret_val->parts, part);
    }

    return ret_val;
}

gboolean
rra_path_trim_front(rra_path *in_path, guint in_parts)
{
    guint i;
    guint avail_parts = rra_path_number_of_parts(in_path);
    if (rra_path_is_absolute(in_path))
        avail_parts++;

    if (in_parts > avail_parts)
    {
        return FALSE;
    }

    if (rra_path_is_absolute(in_path))
    {
        in_path->absolute = FALSE;
        in_parts--;
    }

    for (i = 0; i < in_parts; i++)
    {
        rra_path_part *part = get_part(in_path, 0);

        if (!g_ptr_array_remove(in_path->parts, part))
        {
            return FALSE;
        }

        rra_path_part_free(part);
    }

    rra_path_clear_mutable(in_path);
    return TRUE;
}

gboolean
rra_path_trim_back(rra_path *in_path, guint in_parts)
{
    guint avail_parts = rra_path_number_of_parts(in_path);
    if (rra_path_is_absolute(in_path))
        avail_parts++;

    if (in_parts > avail_parts)
    {
        return FALSE;
    }

    while (rra_path_number_of_parts(in_path) > 0
           && in_parts > 0)
    {
        rra_path_part *part = get_part(in_path, in_path->parts->len - 1);

        if (!g_ptr_array_remove(in_path->parts, part))
        {
            return FALSE;
        }

        rra_path_part_free(part);
        in_parts--;
    }

    if (in_parts == 0)
    {
        rra_path_clear_mutable(in_path);
        return TRUE;
    }
    else if (in_parts == 1)
    {
        in_path->absolute = FALSE;
        rra_path_clear_mutable(in_path);
        return TRUE;
    }
    else
    {
        // This shouldn't happen
        return FALSE;
    }
}
