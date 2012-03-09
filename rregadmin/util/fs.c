/*
 * Authors:	James Lewismoss <jlm@racemi.com>
 *
 * Copyright (c) 2007 Racemi
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/util/fs.h>
#include <rregadmin/util/path.h>
#include <rregadmin/util/log.h>

static gboolean per_path_ff(const rra_path *in_path, rra_path *out_path);

gboolean
find_file_casei_bang(ustring *in_str_path)
{
    if (filesysob_exists(in_str_path))
    {
        return TRUE;
    }
    {
        rra_path *in_path = rra_path_new_unix(ustr_as_utf8(in_str_path));
        rra_path *out_path;
        if (rra_path_is_absolute(in_path))
        {
            out_path = rra_path_new_unix("/");
        }
        else
        {
            out_path = rra_path_new_unix("");
        }

        int i;
        gboolean success = TRUE;

        for (i = 0; i < rra_path_number_of_parts(in_path); i++)
        {
            if (!per_path_ff(in_path, out_path))
            {
                success = FALSE;
                break;
            }
            rra_info(N_("Found path %s"),
                     ustr_as_utf8(rra_path_as_ustr(out_path)));
        }

        if (success)
        {
            ustr_overwrite(in_str_path, rra_path_as_ustr(out_path));
            rra_info(N_("Found path %s"),
                     ustr_as_utf8(rra_path_as_ustr(out_path)));
        }

        rra_path_free(in_path);
        rra_path_free(out_path);

        return success;
    }
}

static gboolean
per_path_ff(const rra_path *in_path, rra_path *out_path)
{
    gboolean ret_val = FALSE;

    ustring *my_part =
        ustr_copy(rra_path_get_part(in_path,
                                    rra_path_number_of_parts(out_path)));

    rra_path_append_part(out_path, ustr_as_utf8(my_part));
    if (filesysob_exists(rra_path_as_ustr(out_path)))
    {
        rra_info(N_("Found path %s"),
                 ustr_as_utf8(rra_path_as_ustr(out_path)));
        return TRUE;
    }

    /* Now check entry by entry for a case insensitive match */
    rra_path_trim_back(out_path, 1);
    const char *dir_path;
    if (rra_path_number_of_parts(out_path) == 0)
    {
        dir_path = ".";
    }
    else
    {
        dir_path = rra_path_as_str(out_path);
    }

    rra_debug(N_("Looking through dir %s"), dir_path);

    GDir *curdir = g_dir_open(dir_path, 0, NULL);
    ustring *dir_entry = ustr_create(g_dir_read_name(curdir));
    ustring *found_entry = NULL;
    while (ustr_strlen(dir_entry) > 0)
    {
        if (ustr_casecmp(dir_entry, my_part) == 0)
        {
            if (found_entry == NULL)
            {
                found_entry = ustr_copy(dir_entry);
            }
            else
            {
                rra_warning(N_("Found duplicate entry: %s vs %s"),
                            ustr_as_utf8(found_entry),
                            ustr_as_utf8(dir_entry));
                ustr_free(found_entry);
                found_entry = NULL;
                break;
            }
        }

        ustr_strset(dir_entry, g_dir_read_name(curdir));
    }

    if (found_entry != NULL)
    {
        rra_path_append_part(out_path, ustr_as_utf8(found_entry));
        rra_info(N_("Found path %s"),
                 ustr_as_utf8(rra_path_as_ustr(out_path)));
        ret_val = TRUE;
        ustr_free(found_entry);
    }

    ustr_free(dir_entry);
    g_dir_close(curdir);

    return ret_val;
}

ustring*
find_file_casei(const ustring *in_path)
{
    ustring *ret_val = ustr_copy(in_path);

    gboolean bang_ret = find_file_casei_bang(ret_val);
    if (bang_ret == TRUE)
    {
        return ret_val;
    }
    else
    {
        ustr_free(ret_val);
        return NULL;
    }
}

gboolean
filesysob_exists(const ustring *in_str)
{
    struct stat fo_stat;

    if (stat(ustr_as_utf8(in_str), &fo_stat) == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

gboolean
filesysob_exists_and_non_zero_sized(const ustring *in_str)
{
    struct stat fo_stat;

    if (stat(ustr_as_utf8(in_str), &fo_stat) == 0)
    {
        if (fo_stat.st_size > 0)
            return TRUE;
        else
            return FALSE;
    }
    else
    {
        return FALSE;
    }
}

#ifndef HAVE_MKDTEMP
static gchar gen_random_char(const gchar *in_choose_list, gint in_list_len);

static gchar
gen_random_char(const gchar *in_choose_list, gint in_list_len)
{
    static GRand *generator = NULL;
    if (generator == NULL)
    {
        generator = g_rand_new();
    }

    return in_choose_list[g_rand_int_range(generator, 0, in_list_len)];
}

static const gchar *ran_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
#endif

gchar*
rra_mkdtemp(const char *in_dir_prefix)
{
#ifdef HAVE_MKDTEMP
    gchar *ret_val = g_strdup_printf("%s-XXXXXX", in_dir_prefix);
    if (mkdtemp(ret_val) == NULL)
    {
        rra_warning(N_("Error creating temp dir %s"), ret_val);
        g_free(ret_val);
        return NULL;
    }
    return ret_val;
#else
    gchar *ret_val = g_strdup_printf("%s-%c%c%c%c%c%c", in_dir_prefix,
                                     gen_random_char(ran_chars,
                                                     G_N_ELEMENTS(ran_chars)),
                                     gen_random_char(ran_chars,
                                                     G_N_ELEMENTS(ran_chars)),
                                     gen_random_char(ran_chars,
                                                     G_N_ELEMENTS(ran_chars)),
                                     gen_random_char(ran_chars,
                                                     G_N_ELEMENTS(ran_chars)),
                                     gen_random_char(ran_chars,
                                                     G_N_ELEMENTS(ran_chars)),
                                     gen_random_char(ran_chars,
                                                     G_N_ELEMENTS(ran_chars)));

    if (mkdir(ret_val, 0700) != 0)
    {
        rra_warning(N_("Error creating temp dir %s: %s"),
                    ret_val, strerror(errno));
        g_free(ret_val);
        return NULL;
    }
    return ret_val;
#endif
}
