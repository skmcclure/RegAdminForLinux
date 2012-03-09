/*
 * Authors:     James LewisMoss <jlm@racemi.com>
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

#include <string.h>
#include <stdio.h>

#include <rregadmin/util/uchar_info.h>
#include <rregadmin/util/binary_search.h>

static gboolean debug = FALSE;

#include "data/uchar_info_data.c"

static gconstpointer
get_char_info_index(gconstpointer in_uci, gint in_index)
{
    const struct unicode_char_info **info =
        (const struct unicode_char_info**)in_uci;
    if (debug)
        fprintf (stderr, "Getting data at index %d\n", in_index);
    return info[in_index];
}

static gint
compare_names(gconstpointer in_a, gconstpointer in_b)
{
    const struct unicode_char_info *a = (const struct unicode_char_info*)in_a;
    const char *name = (const char*)in_b;

    if (in_a == in_b)
    {
        return 0;
    }
    if (a == NULL)
    {
        return -1;
    }
    if (name == NULL)
    {
        return 1;
    }
    if (debug)
        fprintf (stderr, "Comparing %s and %s\n", a->name, name);
    return strcmp(name, a->name);
}

static gint
compare_nums(gconstpointer in_a, gconstpointer in_b)
{
    const struct unicode_char_info *a = (const struct unicode_char_info*)in_a;
    guint32 num = GPOINTER_TO_UINT(in_b);

    if (a == NULL)
    {
        return -1;
    }
    if (debug)
        fprintf (stderr, "Comparing %d and %d\n", a->num, num);
    return num - a->num;
}

const struct unicode_char_info*
uchar_info_by_name(const char *in_name)
{
    initialize_name_info();

    if (debug)
        fprintf (stderr, "Searching for %s between %d and %d\n",
                 in_name, 0, name_sorted_len);

    int index = rra_binary_search(0, name_sorted_len - 1, in_name,
                                  name_sorted_char_info, get_char_info_index,
                                  compare_names, NULL);

    if (index == -1)
    {
        return NULL;
    }

    return name_sorted_char_info[index];
}

const struct unicode_char_info*
uchar_info_by_num(guint32 in_num)
{
    initialize_num_info();

    if (debug)
        fprintf (stderr, "Searching for %d between %d and %d\n",
                 in_num, 0, num_sorted_len);

    int index = rra_binary_search(0, num_sorted_len - 1,
                                  GUINT_TO_POINTER(in_num),
                                  num_sorted_char_info, get_char_info_index,
                                  compare_nums, NULL);

    if (index == -1)
    {
        return NULL;
    }

    return num_sorted_char_info[index];
}

const struct unicode_char_info*
uchar_info_by_index(guint32 in_index)
{
    if (in_index >= name_sorted_len)
    {
        return NULL;
    }

    return name_sorted_char_info[in_index];
}

guint32
uchar_info_get_count(void)
{
    return name_sorted_len;
}
