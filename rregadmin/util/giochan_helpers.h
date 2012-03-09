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

#ifndef RREGADMIN_UTIL_GIOCHAN_HELPERS_H
#define RREGADMIN_UTIL_GIOCHAN_HELPERS_H

#include <stdarg.h>

#include <glib.h>

#include <rregadmin/util/macros.h>

G_BEGIN_DECLS

GIOStatus rra_g_io_channel_printf(GIOChannel *in_chan,
                                  GError **error,
                                  const gchar *in_format,
                                  ...);

GIOStatus rra_g_io_channel_vprintf(GIOChannel *in_chan,
                                   GError **error,
                                   const gchar *in_format,
                                   va_list in_args);


G_END_DECLS

#endif /* RREGADMIN_UTIL_GIOCHAN_HELPERS_H */
