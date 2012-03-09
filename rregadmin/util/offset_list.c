/*
 * Authors:	James LewisMoss <jlm@racemi.com>
 *
 * Copyright 2007 Racemi
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

#include <glib.h>

#include <rregadmin/util/offset_list.h>
#include <rregadmin/util/malloc.h>
#include <rregadmin/util/macros.h>

#define ofs_ptr(in_ofs) GUINT_TO_POINTER(offset_to_begin(in_ofs))

struct offset_list_
{
    offset_holder *holder;
    GTree *offsets;
};

static int allocated_count = 0;

int
ofslist_allocated_count(void)
{
    return allocated_count;
}

static gint ofslist_compare_offsets(gconstpointer a, gconstpointer b);

offset_list*
ofslist_new(offset_holder *in_oh)
{
    offset_list *ret_val;

    ret_val = rra_new_type(offset_list);
    ret_val->holder = in_oh;
    ret_val->offsets = g_tree_new(ofslist_compare_offsets);

    allocated_count++;
    return ret_val;
}

gboolean
ofslist_free(offset_list *in_ol)
{
    if (in_ol == NULL)
    {
        return FALSE;
    }

    g_tree_destroy(in_ol->offsets);

    rra_free_type(offset_list, in_ol);

    allocated_count--;
    return TRUE;
}

gint
ofslist_size(const offset_list *in_ol)
{
    return g_tree_nnodes(in_ol->offsets);
}

offset_holder*
ofslist_get_holder(offset_list *in_ol)
{
    return in_ol->holder;
}

static gint
ofslist_compare_offsets(gconstpointer a, gconstpointer b)
{
    return GPOINTER_TO_UINT(b) - GPOINTER_TO_UINT(a);
}

gboolean
ofslist_add(offset_list *in_ol, offset in_ofs)
{
    if (offset_get_holder(in_ofs) != ofslist_get_holder(in_ol))
    {
        return FALSE;
    }

    if (ofslist_contains(in_ol, in_ofs))
    {
        return TRUE;
    }

    g_tree_insert(in_ol->offsets, ofs_ptr(in_ofs), ofs_ptr(in_ofs));

    return TRUE;
}

gboolean
ofslist_contains(offset_list *in_ol, offset in_ofs)
{
    if (offset_get_holder(in_ofs) != ofslist_get_holder(in_ol))
    {
        return FALSE;
    }

    return g_tree_lookup(in_ol->offsets, ofs_ptr(in_ofs)) != NULL;
}

gboolean
ofslist_remove(offset_list *in_ol, offset in_ofs)
{
    if (offset_get_holder(in_ofs) != ofslist_get_holder(in_ol))
    {
        return FALSE;
    }

    if (ofslist_contains(in_ol, in_ofs))
    {
        g_tree_remove(in_ol->offsets, ofs_ptr(in_ofs));
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
