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

#ifndef RREGADMIN_UTIL_UCHAR_INFO_H
#define RREGADMIN_UTIL_UCHAR_INFO_H 1

#include <glib.h>

G_BEGIN_DECLS

struct unicode_char_info
{
    const char *name;
    const char *descrip;
    const char *origin;
    guint32 num;
};

const struct unicode_char_info* uchar_info_by_name(const char *in_name);

const struct unicode_char_info* uchar_info_by_num(guint32 in_num);

const struct unicode_char_info* uchar_info_by_index(guint32 in_index);

guint32 uchar_info_get_count(void);


G_END_DECLS

#endif // RREGADMIN_UTIL_UCHAR_INFO_H
