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

#include <stdio.h>

#include <glib.h>

#include <rregadmin/util/ustring_list.h>
#include <rregadmin/util/log.h>

/** List of ustrings.
 *
 * @ingroup ustring_list_util_group
 */
struct ustring_list_
{
    /** Magic signature.
     */
    guint32 signature;

    /** List for ustrings.
     */
    GPtrArray *strs;
};

const guint32 USTRLIST_SIG = 0xfe0d1142;

static int allocated_count = 0;

int
ustrlist_allocated_count(void)
{
    return allocated_count;
}

ustring_list*
ustrlist_new(void)
{
    ustring_list *ret_val = g_new0(ustring_list, 1);
    ret_val->signature = USTRLIST_SIG;
    ret_val->strs = g_ptr_array_new();
    allocated_count++;
    return ret_val;
}

ustring_list*
ustrlist_new_list(ustring *in_str, ...)
{
    va_list ap;
    ustring_list *ret_val = ustrlist_new();

    if (in_str == NULL)
    {
        return ret_val;
    }

    if (!ustrlist_append(ret_val, in_str))
    {
        ustrlist_free(ret_val);
        return NULL;
    }

    va_start(ap, in_str);

    int index = 2;
    ustring *da_str = va_arg(ap, ustring*);
    while (da_str != NULL)
    {
        if (!ustr_is_valid(da_str))
        {
            rra_warning("Invalid argument %d: not a ustring", index);
            ustrlist_free(ret_val);
            return NULL;
        }

        if (!ustrlist_append(ret_val, da_str))
        {
            ustrlist_free(ret_val);
            return NULL;
        }

        index++;
        da_str = va_arg(ap, ustring*);
    }

    va_end(ap);

    return ret_val;
}

ustring_list*
ustrlist_copy (const ustring_list *in_ul)
{
    if (in_ul == NULL)
    {
        return NULL;
    }

    ustring_list *ret_val = ustrlist_new();
    int i;

    for (i = 0; i < ustrlist_size(in_ul); i++)
    {
        ustrlist_append(ret_val, ustr_copy(ustrlist_get(in_ul, i)));
    }

    return ret_val;
}

gboolean
ustrlist_is_valid (const ustring_list *in_ul)
{
    if (in_ul == NULL)
    {
        return FALSE;
    }

    if (in_ul->signature != USTRLIST_SIG)
    {
        return FALSE;
    }

    if (in_ul->strs == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

gboolean
ustrlist_free (ustring_list *in_ul)
{
    if (in_ul == NULL)
    {
        return FALSE;
    }

    ustrlist_clear(in_ul);
    g_ptr_array_free(in_ul->strs, TRUE);
    g_free(in_ul);
    allocated_count--;
    return TRUE;
}

gboolean
ustrlist_clear (ustring_list *in_ul)
{
    if (in_ul == NULL)
    {
        return FALSE;
    }

    if (ustrlist_size(in_ul) > 0)
    {
        while (ustrlist_size(in_ul) > 0)
        {
            int index = in_ul->strs->len - 1;
            ustring *u = (ustring*)g_ptr_array_index(in_ul->strs, index);
            ustr_free(u);
            g_ptr_array_remove_index(in_ul->strs, index);
        }
        return TRUE;
    }

    return FALSE;
}

int
ustrlist_size (const ustring_list *in_ul)
{
    if (in_ul == NULL)
    {
        return -1;
    }

    return in_ul->strs->len;
}

const ustring*
ustrlist_get(const ustring_list *in_ul, int in_index)
{
    if (in_ul == NULL)
    {
        return NULL;
    }

    if (in_index >= ustrlist_size(in_ul))
    {
        return NULL;
    }

    return (const ustring*)g_ptr_array_index(in_ul->strs, in_index);
}

gboolean
ustrlist_append(ustring_list *in_ul, ustring *in_str)
{
    if (in_ul == NULL
        || in_str == NULL)
    {
        return FALSE;
    }

    g_ptr_array_add(in_ul->strs, in_str);

    return TRUE;
}

gboolean
ustrlist_equal(const ustring_list *in_ul1, const ustring_list *in_ul2)
{
    return ustrlist_compare(in_ul1, in_ul2) == 0;
}

int
ustrlist_compare(const ustring_list *in_ul1, const ustring_list *in_ul2)
{
    if (in_ul1 == in_ul2)
        return 0;
    if (!in_ul1)
        return -1;
    if (!in_ul2)
        return 1;

    int ret_val = ustrlist_size(in_ul1) - ustrlist_size(in_ul2);
    int i;
    int min = MIN(ustrlist_size(in_ul1), ustrlist_size(in_ul2));

    for (i = 0 ; i < min; i++)
    {
        int comp = ustr_compare(ustrlist_get(in_ul1, i),
                                ustrlist_get(in_ul2, i));
        if (comp != 0)
        {
            return comp;
        }
    }

    return ret_val;
}

void
ustrlist_debug_print(const ustring_list *in_ul)
{
    int i;
    fprintf (stderr, "ustring_list(");

    for (i = 0; i < ustrlist_size(in_ul); i++)
    {
        const ustring *u = ustrlist_get(in_ul, i);
        fprintf (stderr, "%p[%s], ", u, ustr_as_utf8(u));
    }

    fprintf (stderr, ")\n");
}
