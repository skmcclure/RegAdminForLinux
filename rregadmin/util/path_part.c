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

#include <rregadmin/config.h>

#include <string.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/util/path_part.h>
#include <rregadmin/util/malloc.h>
#include <rregadmin/util/log.h>

struct rra_path_part_
{
    rra_path_part_type type;
    ustring *value;
    ustring *encoded;

    const rra_path_info *pi;
};


rra_path_part*
rra_path_part_new(const rra_path_info *in_pi)
{
    if (in_pi == NULL)
    {
        return NULL;
    }

    rra_path_part *ret_val = rra_new_type(rra_path_part);
    ret_val->type = RRA_PATH_PART_EMPTY;
    ret_val->value = ustr_new();
    ret_val->encoded = ustr_new();
    ret_val->pi = in_pi;
    return ret_val;
}

rra_path_part*
rra_path_part_new_element(const rra_path_info *in_pi, const char *in_part)
{
    if (rra_path_info_contains_delim(in_pi, in_part))
    {
        return NULL;
    }

    rra_path_part *ret_val = rra_path_part_new(in_pi);
    if (!rra_path_part_grab_token(ret_val, &in_part))
    {
        rra_path_part_free(ret_val);
        return NULL;
    }

    return ret_val;
}

rra_path_part*
rra_path_part_copy(const rra_path_part *in_pp)
{
    if (in_pp == NULL)
    {
        return NULL;
    }

    rra_path_part *ret_val = rra_path_part_new(in_pp->pi);
    ret_val->type = in_pp->type;
    if (in_pp->value != NULL)
    {
        rra_debug("Copying value");
        ustr_overwrite(ret_val->value, in_pp->value);
    }
    if (in_pp->encoded != NULL)
    {
        rra_debug("Copying encoded");
        ustr_overwrite(ret_val->encoded, in_pp->encoded);
    }
    return ret_val;
}

gboolean
rra_path_part_free(rra_path_part *in_pp)
{
    ustr_free(in_pp->value);
    in_pp->value = NULL;

    ustr_free(in_pp->encoded);
    in_pp->encoded = NULL;

    rra_free_type(rra_path_part, in_pp);
    return TRUE;
}

gboolean
rra_path_part_clear(rra_path_part *in_pp)
{
    ustr_clear(in_pp->value);
    ustr_clear(in_pp->encoded);
    in_pp->type = RRA_PATH_PART_EMPTY;

    return TRUE;
}

int
rra_path_part_compare(rra_path_part *in_pp1, rra_path_part *in_pp2)
{
    if (in_pp1 == in_pp2)
    {
        return 0;
    }
    if (in_pp1 == NULL)
    {
        return -1;
    }
    if (in_pp2 == NULL)
    {
        return 1;
    }

    if (rra_path_info_has_option(in_pp1->pi, RRA_PATH_OPT_CASE_INSENSITIVE)
        || rra_path_info_has_option(in_pp2->pi, RRA_PATH_OPT_CASE_INSENSITIVE))
    {
        return ustr_casecmp(rra_path_part_get_value(in_pp1),
                            rra_path_part_get_value(in_pp2));
    }
    else
    {
        return ustr_compare(rra_path_part_get_value(in_pp1),
                            rra_path_part_get_value(in_pp2));
    }

    return 0;
}

gboolean
rra_path_part_grab_token(rra_path_part *in_pp, const char **in_str)
{
    if (!rra_path_part_clear(in_pp))
    {
        return FALSE;
    }

    while (**in_str != '\0')
    {
        if (rra_path_info_is_delim(in_pp->pi, *in_str))
        {
            if (ustr_strlen(in_pp->value) == 0)
            {
                in_pp->type = RRA_PATH_PART_DELIM;
                ustr_strset(in_pp->value, rra_path_info_get_delim(in_pp->pi));
                (*in_str)++;
            }
            break;
        }
        else
        {
            ustr_charcat(in_pp->value, **in_str);
        }

        (*in_str)++;
    }

    if (in_pp->type == RRA_PATH_PART_EMPTY)
    {
        if (ustr_strlen(in_pp->value) == 0)
        {
            in_pp->type = RRA_PATH_PART_EMPTY;
        }
        else if (strcmp(".", ustr_as_utf8(in_pp->value)) == 0)
        {
            in_pp->type = RRA_PATH_PART_CURRENT;
        }
        else if (strcmp("..", ustr_as_utf8(in_pp->value)) == 0)
        {
            in_pp->type = RRA_PATH_PART_PARENT;
        }
        else
        {
            in_pp->type = RRA_PATH_PART_ELEMENT;
        }
    }

    return TRUE;
}

rra_path_part_type
rra_path_part_get_type(const rra_path_part *in_pp)
{
    if (in_pp == NULL)
    {
        return RRA_PATH_PART_ERROR;
    }

    return in_pp->type;
}

const ustring*
rra_path_part_get_value(const rra_path_part *in_pp)
{
    if (in_pp == NULL
        || ustr_strlen(in_pp->value) == 0)
    {
        return NULL;
    }

    return in_pp->value;
}
