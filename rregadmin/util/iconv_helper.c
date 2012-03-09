/*
 * Authors:     Sean Loaring
 *              Petter Nordahl-Hagen
 *              James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi Inc
 * Copyright (c) 2005-2006 Sean Loaring
 * Copyright (c) 1997-2004 Petter Nordahl-Hagen
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

#include <rregadmin/util/iconv_helper.h>
#include <rregadmin/util/log.h>
#include <rregadmin/util/malloc.h>

struct iconv_helper_
{
    GHashTable *convs;
};

typedef GPtrArray* iconv_t_list;

static void destroy_iconv_t_list (gpointer data);
static iconv_t_list create_iconv_t_list (void);


iconv_helper*
iconv_helper_new (void)
{
    iconv_helper *ret_val = rra_new_sizeof(iconv_helper);
    ret_val->convs = g_hash_table_new_full(g_str_hash, g_str_equal, g_free,
                                           destroy_iconv_t_list);
    return ret_val;
}

struct iconv_t_wrapper
{
    iconv_t it;
    gboolean used;
};

void
destroy_iconv_t_list (gpointer data)
{
    iconv_t_list it_list = (iconv_t_list)data;

    while (it_list->len > 0)
    {
        struct iconv_t_wrapper* w =
            (struct iconv_t_wrapper*)g_ptr_array_remove_index_fast(it_list, 0);
        if (w->used)
        {
            rra_warning(N_("Freeing an unreleased iconv_t"));
        }
        iconv_close(w->it);

        w->it = 0;
        g_free(w);
    }
}

iconv_t_list
create_iconv_t_list (void)
{
    return g_ptr_array_new();
}

gboolean
iconv_helper_free (iconv_helper *in_helper)
{
    if (in_helper == NULL)
    {
        return FALSE;
    }

    g_hash_table_destroy(in_helper->convs);
    rra_free(sizeof(iconv_helper), in_helper);
    return TRUE;
}

#define MAKE_KEY(key_name, to, from)            \
    char key_name[512];                         \
    strcpy(key, from);                          \
    strcat(key, "->");                          \
    strcat(key, to)

#define COPY_KEY(key_name)                      \
    g_strdup(key_name)

#define RELEASE_KEY(name)

iconv_t
iconv_helper_get(iconv_helper *in_helper,
                 const char *in_to_coding, const char *in_from_coding)
{
    int i;
    MAKE_KEY(key, in_to_coding, in_from_coding);
    iconv_t_list it_list =
        (iconv_t_list)g_hash_table_lookup(in_helper->convs, key);

    /* Make sure list exists */
    if (it_list == NULL)
    {
        it_list = create_iconv_t_list();
        g_hash_table_insert (in_helper->convs, COPY_KEY(key), it_list);
    }
    else
    {
        RELEASE_KEY(key);
    }

    /* Search for available */
    for (i = 0; i < it_list->len; i++)
    {
        struct iconv_t_wrapper* w =
            (struct iconv_t_wrapper*)g_ptr_array_index(it_list, i);

        if (!w->used)
        {
            w->used = TRUE;
            /* rra_debug("Returning iconv_t: %p", w->it); */
            return w->it;
        }
    }

    /* no unused available */
    struct iconv_t_wrapper* itw = g_new(struct iconv_t_wrapper, 1);
    itw->used = TRUE;
    itw->it = iconv_open(in_to_coding, in_from_coding);

    if (itw->it == (iconv_t)-1)
    {
        rra_warning(N_("Got invalid iconv_t from iconv_open"));
        return (iconv_t)-1;
    }

    g_ptr_array_add(it_list, itw);

    /* rra_debug("Returning iconv_t: %p", itw->it); */

    return itw->it;
}

gboolean
iconv_helper_release (iconv_helper *in_helper, iconv_t in_it,
                      const char *in_to_coding, const char *in_from_coding)
{
    int i;

    /* rra_debug("Releasing: %p", in_it); */

    /* Reset converter */
    iconv(in_it, NULL, NULL, NULL, NULL);

    MAKE_KEY(key, in_to_coding, in_from_coding);
    iconv_t_list it_list =
        (iconv_t_list)g_hash_table_lookup(in_helper->convs, key);
    RELEASE_KEY(key);

    if (it_list == NULL)
    {
        rra_warning(N_("This shouldn't happen"));
    }

    for (i = 0; i < it_list->len; i++)
    {
        struct iconv_t_wrapper* w =
            (struct iconv_t_wrapper*)g_ptr_array_index(it_list, i);

        if (w->it == in_it)
        {
            if (!w->used)
            {
                rra_warning(N_("Double release of iconv_t: %p"), w->it);
            }
            else
            {
                w->used = FALSE;
                return TRUE;
            }
        }
    }

    rra_warning(N_("Didn't find iconv_t to release"));
    return FALSE;
}
