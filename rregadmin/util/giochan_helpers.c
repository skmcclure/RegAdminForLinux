/*
 * Authors:	James Lewismoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi
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

#include <rregadmin/util/giochan_helpers.h>

GIOStatus
rra_g_io_channel_printf(GIOChannel *in_chan, GError **error,
                        const gchar *in_format, ...)
{
    GIOStatus ret_val;
    va_list args;
    va_start(args, in_format);
    ret_val = rra_g_io_channel_vprintf(in_chan, error, in_format, args);
    va_end(args);
    return ret_val;
}

GIOStatus
rra_g_io_channel_vprintf(GIOChannel *in_chan, GError **error,
                         const gchar *in_format, va_list in_args)
{
    if (in_chan == NULL)
    {
        return G_IO_STATUS_EOF;
    }

    GIOStatus ret_val;
    gchar *msg = g_strdup_vprintf(in_format, in_args);
    ret_val = g_io_channel_write_chars(in_chan, msg, strlen(msg),
                                       NULL, error);
    g_free(msg);
    g_io_channel_flush(in_chan, NULL);
    return ret_val;
}
