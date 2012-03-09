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

#include <rregadmin/util/conversion_utils.h>
#include <rregadmin/util/log.h>
#include <rregadmin/util/text_utils.h>
#include <rregadmin/util/intutils.h>

#ifndef HAVE_G_ASCII_STRTOLL
#include <stdlib.h>
#define g_ascii_strtoll strtoll
#endif

gboolean
fuzzy_str_to_dword(const char *in_str, dword_type *out_val)
{
    qword_type tmp_val;
    if (!fuzzy_str_to_qword(in_str, &tmp_val))
    {
        return FALSE;
    }

    if (tmp_val < 0 || tmp_val > G_MAXUINT32)
    {
        return FALSE;
    }

    *out_val = (dword_type)tmp_val;
    return TRUE;
}

static gboolean
contains_hex_digit(const char *in_str)
{
    int i;
    for (i = 0; in_str[i] != '\0'; i++)
    {
        if (isxdigit(in_str[i]) && !isdigit(in_str[i]))
        {
            return TRUE;
        }
    }
    return FALSE;
}

gboolean
fuzzy_str_to_qword(const char *in_str, qword_type *out_val)
{
    gint64 tmp_val;

    char *endptr = NULL;

    if (in_str == NULL || out_val == NULL)
    {
        return FALSE;
    }

    const char *str_ptr = in_str;

    skip_whitespace(&str_ptr);

    errno = 0;
    if (g_str_has_prefix(str_ptr, "0x"))
    {
        tmp_val = g_ascii_strtoll(in_str, &endptr, 16);
    }
    else if (contains_hex_digit(str_ptr))
    {
        tmp_val = g_ascii_strtoll(in_str, &endptr, 16);
    }
    else
    {
        tmp_val = g_ascii_strtoll(in_str, &endptr, 10);

        if (errno == ERANGE)
        {
            errno = 0;
            tmp_val = g_ascii_strtoll(in_str, &endptr, 16);
        }
    }

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
check_convert_results(const char *in_str, char *endptr)
{
    if (endptr == in_str)
    {
        rra_debug("%s(): No number found in string: '%s'",
                  __func__, in_str);
        return FALSE;
    }

    if (endptr != NULL)
    {
        if (errno == ERANGE)
        {
            rra_debug("%s(): integer in string '%s' out of range",
                      __func__, in_str);
            return FALSE;
        }

        skip_whitespace((const char**)&endptr);
        if (endptr != (in_str + strlen(in_str)))
        {
            rra_debug("%s(): Non space at end of string(%s): '%s'",
                      __func__, in_str, endptr);
            return FALSE;
        }
    }

    return TRUE;
}

gboolean
check_min_max(gint64 in_val, gint64 in_min, gint64 in_max)
{
    if (in_val < in_min || in_val > in_max)
    {
        return FALSE;
    }
    return TRUE;
}


static int
hex_dig_value(char in_c)
{
    switch (in_c)
    {
    case '0':
        return 0;
    case '1':
        return 1;
    case '2':
        return 2;
    case '3':
        return 3;
    case '4':
        return 4;
    case '5':
        return 5;
    case '6':
        return 6;
    case '7':
        return 7;
    case '8':
        return 8;
    case '9':
        return 9;
    case 'a':
    case 'A':
        return 10;
    case 'b':
    case 'B':
        return 11;
    case 'c':
    case 'C':
        return 12;
    case 'd':
    case 'D':
        return 13;
    case 'e':
    case 'E':
        return 14;
    case 'f':
    case 'F':
        return 15;
    default:
        return -1;
    }
}

static int
generate_guint8(const char **in_str)
{
    char first;
    char second;

    if (!isxdigit(**in_str))
    {
        return -1;
    }

    first = **in_str;
    (*in_str)++;

    skip_whitespace(in_str);

    if (!isxdigit(**in_str))
    {
        second = first;
        first = '0';
    }
    else
    {
        second = **in_str;
        (*in_str)++;
    }

    skip_whitespace(in_str);

    return (16 * hex_dig_value(first)) + hex_dig_value(second);
}

gboolean
str_to_binary(const char *in_str, GByteArray *out_val)
{
    const char *req_ending = NULL;

    if (in_str == NULL || out_val == NULL)
    {
        return FALSE;
    }

    const char *str_ptr = in_str;

    skip_whitespace(&str_ptr);

    if (g_str_has_prefix(str_ptr, "0x"))
    {
        str_ptr += 2;
    }
    else if (g_str_has_prefix(str_ptr, "HEX"))
    {
        str_ptr += 3;
        skip_whitespace(&str_ptr);
        if (str_ptr[0] != '(')
        {
            return FALSE;
        }
        str_ptr++;
        req_ending = ")";
    }
    else
    {
        return FALSE;
    }

    skip_whitespace(&str_ptr);

    /* Now we should be up to the first hex digit */

    int num = generate_guint8(&str_ptr);
    while (num != -1)
    {
        guint8 tmp_byte = (guint8)num;
        g_byte_array_append(out_val, &tmp_byte, 1);

        num = generate_guint8(&str_ptr);
    }

    skip_whitespace(&str_ptr);
    if (req_ending != NULL)
    {
        if (!g_str_has_prefix(str_ptr, req_ending))
        {
            return FALSE;
        }

        str_ptr += strlen(req_ending);
    }
    skip_whitespace(&str_ptr);

    if (str_ptr != (in_str + strlen(in_str)))
    {
        return FALSE;
    }

    return TRUE;
}
