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

#ifndef RREGADMIN_UTIL_BINARY_SEARCH_H
#define RREGADMIN_UTIL_BINARY_SEARCH_H

#include <glib.h>
#include <rregadmin/util/macros.h>

G_BEGIN_DECLS

/** Typedef for a function to retrieve a value by index.
 *
 * Used in rra_binary_search() and rra_binary_search_gptrarray().
 *
 * @ingroup util_group
 */
typedef gconstpointer (*RRARetrieveFunc) (gconstpointer retrieve_data,
                                         gint index);

/** Generic binary search algorithm.
 *
 * This implements a generic binary search algorithm.  It searchs from
 * start to stop using the r_func to retrieve the values and the c_func
 * to compare the values.  If you need to pass extra data to the
 * retrieve function you can use retrieve_data and extra data for the
 * compare func in compare_data.
 *
 * Whether the value is found or not one_before is set to the item one
 * before the item or where the item would be (easing inserting a value
 * in the middle).
 *
 * @param start  the first item's index
 * @param stop   the last item's index
 * @param compare_data  data passed to the c_func
 * @param retrieve_data data passed to the retrieve func
 * @param r_func  function pointer taking retrieve_data and an index and
 *                returning a const void pointer.
 * @param c_func  function taking two pointers as returned from r_func
 *                and the compare_data and doing a comparison on it
 * @param one_before the index one before the item found or where the item
 *                   would be.
 * @return the index for the item or -1 if the item isn't found.
 *
 * @ingroup util_group
 */
gint rra_binary_search(gint start, gint stop, gconstpointer compare_data,
                       gconstpointer retrieve_data, RRARetrieveFunc r_func,
                       GCompareFunc c_func, gint *one_before);

/** Generic binary search algorithm through a GPtrArray.
 *
 * This implements a generic binary search algorithm.  It searchs through
 * the GPtrArray using the c_func to compare the provided value,
 * compare_data, with the data in the array.
 *
 * @note The GPtrArray _must_ be sorted before being passed to this function
 *       or a possibly endless loop could occur.
 *
 * Whether the value is found or not one_before is set to the item one
 * before the item or where the item would be (easing inserting a value
 * in the middle).
 *
 * @param in_arr  The array holding the data.
 * @param compare_data  data passed to the c_func
 * @param c_func  function taking two pointers as returned from r_func
 *                and the compare_data and doing a comparison on it
 * @param one_before the index one before the item found or where the item
 *                   would be.
 *
 * @return the index for the item or -1 if the item isn't found.
 *
 * @ingroup util_group
 */
gint rra_binary_search_gptrarray(GPtrArray *in_arr, gconstpointer compare_data,
                                 GCompareFunc c_func, gint *one_before);

G_END_DECLS

#endif /* RREGADMIN_UTIL_BINARY_SEARCH_H */
