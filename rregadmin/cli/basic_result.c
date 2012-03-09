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

#include <glib/gmem.h>

#include <rregadmin/cli/basic_result.h>

#include <rregadmin/util/ustring.h>
#include <rregadmin/util/malloc.h>

typedef enum {
    HTBR_CLEAR   = 0x00010000,
    HTBR_ERROR   = 0x00100000,
    HTBR_SUCCESS = 0x01000000,
    HTBR_EXIT    = 0x10000000,
//     HTBR_SUCCESS_NONE = HTBR_SUCCESS | 0x00000001,
//     HTBR_SUCCESS_STRING = HTBR_SUCCESS | 0x00000002,
//     HTBR_SUCCESS_BINARY = HTBR_SUCCESS | 0x00000003
} htbr_type;

/** Structur for the result data.
 *
 * @internal
 *
 * @ingroup basic_result_cli_group
 */
struct RRACliResult_
{
    htbr_type type;
    ustring *content;
    GByteArray *data;
    Cell *cell;
};

RRACliResult*
rra_cli_result_new(void)
{
    RRACliResult *ret_val = rra_new_type(RRACliResult);
    ret_val->type = HTBR_CLEAR;
    ret_val->content = NULL;
    ret_val->data = NULL;
    ret_val->cell = NULL;
    return ret_val;
}

gboolean
rra_cli_result_free(RRACliResult *in_res)
{
    if (in_res == NULL)
    {
        return FALSE;
    }

    if (in_res->content != NULL)
    {
        ustr_free(in_res->content);
        in_res->content = NULL;
    }

    if (in_res->data != NULL)
    {
        g_byte_array_free(in_res->data, TRUE);
        in_res->data = NULL;
    }

    rra_free_type(RRACliResult, in_res);
    return TRUE;
}

gboolean
rra_cli_result_set_error(RRACliResult *in_res)
{
    if (in_res == NULL)
    {
        return FALSE;
    }

    if (rra_cli_result_is_error(in_res))
    {
        return TRUE;
    }

    if (rra_cli_result_is_success(in_res))
    {
        if (in_res->content != NULL)
        {
            ustr_free(in_res->content);
            in_res->content = NULL;
        }
        if (in_res->data != NULL)
        {
            g_byte_array_free(in_res->data, TRUE);
            in_res->data = NULL;
        }
        in_res->cell = NULL;
    }

    in_res->type = HTBR_ERROR;

    return TRUE;
}

gboolean
rra_cli_result_set_success(RRACliResult *in_res)
{
    if (in_res == NULL)
    {
        return FALSE;
    }

    if (rra_cli_result_is_error(in_res))
    {
        return FALSE;
    }

    if (rra_cli_result_is_success(in_res))
    {
        return TRUE;
    }

    in_res->type = HTBR_SUCCESS;

    return TRUE;
}

gboolean
rra_cli_result_set_exit(RRACliResult *in_res)
{
    if (in_res == NULL)
    {
        return FALSE;
    }

    if (rra_cli_result_is_error(in_res))
    {
        return FALSE;
    }

    if (rra_cli_result_is_success(in_res))
    {
        return FALSE;
    }

    in_res->type = HTBR_EXIT;

    return TRUE;
}

gboolean
rra_cli_result_is_error(const RRACliResult *in_res)
{
    if (in_res == NULL)
    {
        return TRUE;
    }

    return in_res->type == HTBR_ERROR;
}

gboolean
rra_cli_result_is_success(const RRACliResult *in_res)
{
    if (in_res == NULL)
    {
        return FALSE;
    }

    return (in_res->type & HTBR_SUCCESS) > 0;
}

gboolean
rra_cli_result_is_exit(const RRACliResult *in_res)
{
    if (in_res == NULL)
    {
        return FALSE;
    }

    return (in_res->type & HTBR_EXIT) > 0;
}

gboolean
rra_cli_result_has_content(const RRACliResult *in_res)
{
    if (in_res == NULL)
    {
        return FALSE;
    }

    return in_res->content != NULL;
}

gboolean
rra_cli_result_has_data(const RRACliResult *in_res)
{
    if (in_res == NULL)
    {
        return FALSE;
    }

    return in_res->data != NULL;
}

static gboolean
add_content(RRACliResult *in_res, const char *in_message, va_list args)
{
    if (in_res->content == NULL)
    {
        in_res->content = ustr_new();
    }

    if (ustr_strlen(in_res->content) > 0)
    {
        ustr_strcat(in_res->content, "\n");
    }

    ustr_vprintfa(in_res->content, in_message, args);

    return TRUE;
}

static gboolean
add_content_simple(RRACliResult *in_res, const char *in_message)
{
    if (in_res->content == NULL)
    {
        in_res->content = ustr_new();
    }

    if (ustr_strlen(in_res->content) > 0)
    {
        ustr_strcat(in_res->content, "\n");
    }

    ustr_strcat (in_res->content, in_message);

    return TRUE;
}

gboolean
rra_cli_result_add_error(RRACliResult *in_res, const char *in_message, ...)
{
    if (in_res == NULL)
    {
        return FALSE;
    }

    rra_cli_result_set_error(in_res);

    gboolean ret_val;
    va_list ap;
    va_start (ap, in_message);
    ret_val = add_content(in_res, in_message, ap);
    va_end (ap);
    return ret_val;
}

gboolean
rra_cli_result_add_message(RRACliResult *in_res,
                             const char *in_message)
{
    if (in_res == NULL)
    {
        return FALSE;
    }

    if (!rra_cli_result_set_success(in_res))
    {
        return FALSE;
    }

    add_content_simple (in_res, in_message);

    return TRUE;
}

gboolean
rra_cli_result_set_data(RRACliResult *in_res, GByteArray *in_data)
{
    if (in_res == NULL)
    {
        return FALSE;
    }

    if (!rra_cli_result_set_success(in_res))
    {
        return FALSE;
    }

    if (in_res->data != NULL)
    {
        g_byte_array_free(in_res->data, TRUE);
    }

    in_res->data = in_data;

    return TRUE;
}

gboolean
rra_cli_result_set_cell(RRACliResult *in_res, Cell *in_cell)
{
    if (in_res == NULL)
    {
        return FALSE;
    }

    if (!rra_cli_result_set_success(in_res))
    {
        return FALSE;
    }

    in_res->cell = in_cell;

    return TRUE;
}

const char*
rra_cli_result_get_content(const RRACliResult *in_res)
{
    if (in_res == NULL || in_res->content == NULL)
    {
        return "";
    }
    else
    {
        return ustr_as_utf8(in_res->content);
    }
}

const GByteArray*
rra_cli_result_get_data(const RRACliResult *in_res)
{
    if (in_res == NULL)
    {
        return NULL;
    }

    return in_res->data;
}

Cell*
rra_cli_result_get_cell(const RRACliResult *in_res)
{
    if (in_res == NULL)
    {
        return NULL;
    }

    return in_res->cell;
}
