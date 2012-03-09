/*
 * Authors:	James Lewismoss <jlm@racemi.com>
 *
 * Copyright 2006,2008 Racemi
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
#include <stdio.h>

#include <rregadmin/util/binary_search.h>

static gconstpointer get_gptrarray_item(gconstpointer retrieve_data,
                                        gint index);

gint
rra_binary_search(gint start, gint stop, gconstpointer compare_data,
                  gconstpointer retrieve_data, RRARetrieveFunc r_func,
                  GCompareFunc c_func, gint *one_before)
{
    gint chkval;

    if (stop == -1)
    {
        if (one_before)
        {
            *one_before = stop;
        }
        return -1;
    }

    if (start > stop)
    {
        if (one_before)
        {
            *one_before = stop;
        }
        return -1;
    }

    chkval = (start + stop) / 2;

    int c = c_func(r_func(retrieve_data, chkval), compare_data);

    // chkval < name
    if (c > 0)
    {
        return rra_binary_search(chkval + 1, stop, compare_data,
                                retrieve_data, r_func, c_func, one_before);
    }
    // chkval > name
    else if (c < 0)
    {
        return rra_binary_search(start, chkval - 1, compare_data,
                                retrieve_data, r_func, c_func, one_before);
    }
    else /* c == 0 */
    {
        if (one_before)
        {
            *one_before = chkval - 1;
        }
        return chkval;
    }
}

static gconstpointer
get_gptrarray_item(gconstpointer retrieve_data, gint index)
{
    return g_ptr_array_index((GPtrArray*)retrieve_data, index);
}

gint
rra_binary_search_gptrarray(GPtrArray *in_arr,
                            gconstpointer compare_data,
                            GCompareFunc c_func,
                            gint *one_before)
{
    return rra_binary_search(0,
                             in_arr->len - 1,
                             compare_data,
                             in_arr,
                             get_gptrarray_item,
                             c_func,
                             one_before);
}
