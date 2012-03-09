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

#include <rregadmin/config.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/util/intutils.h>
#include <rregadmin/util/log.h>
#include <rregadmin/util/text_utils.h>
#include <rregadmin/util/conversion_utils.h>

#ifndef HAVE_G_ASCII_STRTOLL
#include <stdlib.h>
#define g_ascii_strtoll strtoll
#endif

gboolean
str_to_gint8(const char *in_str, gint8 *out_val)
{
    gint32 tmp_val;
    if (!str_to_gint32(in_str, &tmp_val))
    {
        return FALSE;
    }

    if (!check_min_max(tmp_val, G_MININT8, G_MAXINT8))
    {
        return FALSE;
    }

    *out_val = (gint8)tmp_val;
    return TRUE;
}

gboolean
str_to_gint16(const char *in_str, gint16 *out_val)
{
    gint32 tmp_val;
    if (!str_to_gint32(in_str, &tmp_val))
    {
        return FALSE;
    }

    if (!check_min_max(tmp_val, G_MININT16, G_MAXINT16))
    {
        return FALSE;
    }

    *out_val = (gint16)tmp_val;
    return TRUE;
}

gboolean
str_to_gint32(const char *in_str, gint32 *out_val)
{
    gint64 tmp_val;
    if (!str_to_gint64(in_str, &tmp_val))
    {
        return FALSE;
    }

    if (!check_min_max(tmp_val, G_MININT32, G_MAXINT32))
    {
        return FALSE;
    }

    *out_val = (gint32)tmp_val;
    return TRUE;
}

gboolean
str_to_gint64(const char *in_str, gint64 *out_val)
{
    gint64 tmp_val;

    char *endptr = NULL;

    if (in_str == NULL || out_val == NULL)
    {
        return FALSE;
    }

    errno = 0;
    tmp_val = g_ascii_strtoll(in_str, &endptr, 0);

    if (errno == ERANGE)
    {
        return FALSE;
    }

    if (!check_convert_results(in_str, endptr))
    {
        return FALSE;
    }

    *out_val = tmp_val;

    return TRUE;
}

gboolean
str_to_guint8(const char *in_str, guint8 *out_val)
{
    gint32 tmp_val;
    if (!str_to_gint32(in_str, &tmp_val))
    {
        return FALSE;
    }

    if (!check_min_max(tmp_val, 0, G_MAXUINT8))
    {
        return FALSE;
    }

    *out_val = (guint8)tmp_val;
    return TRUE;
}

gboolean
str_to_guint16(const char *in_str, guint16 *out_val)
{
    gint32 tmp_val;
    if (!str_to_gint32(in_str, &tmp_val))
    {
        return FALSE;
    }

    if (!check_min_max(tmp_val, 0, G_MAXUINT16))
    {
        return FALSE;
    }

    *out_val = (guint16)tmp_val;
    return TRUE;
}

gboolean
str_to_guint32(const char *in_str, guint32 *out_val)
{
    gint64 tmp_val;
    if (!str_to_gint64(in_str, &tmp_val))
    {
        return FALSE;
    }

    if (tmp_val < 0 || tmp_val > G_MAXUINT32)
    {
        return FALSE;
    }

    *out_val = (guint32)tmp_val;
    return TRUE;
}

gboolean
str_to_guint64(const char *in_str, guint64 *out_val)
{
    guint64 tmp_val;

    char *endptr = NULL;

    if (in_str == NULL || out_val == NULL)
    {
        return FALSE;
    }

    /*
     * OK. Negative values are accepted by strtoull and silently
     * converted to positive ones so we need to check here for a
     * leading - (not followed by a 0)
     */
    const char *tmpptr = in_str;
    skip_whitespace(&tmpptr);
    if (tmpptr[0] == '-' && tmpptr[1] != '0')
    {
        rra_debug("%s(): negative number passed: '%s'",
                  __func__, in_str);
        return FALSE;
    }

    errno = 0;
    tmp_val = g_ascii_strtoull(in_str, &endptr, 0);
    if (errno == ERANGE)
    {
        return FALSE;
    }

    if (!check_convert_results(in_str, endptr))
    {
        return FALSE;
    }

    *out_val = tmp_val;

    return TRUE;
}
