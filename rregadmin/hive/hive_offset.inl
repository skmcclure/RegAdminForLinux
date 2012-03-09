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

G_INLINE_FUNC Hive*
hive_offset_get_hive(hive_offset *ho)
{
    return ho->hv;
}

G_INLINE_FUNC offset
hive_offset_get_offset(hive_offset *ho)
{
    return ho->ofs;
}

G_INLINE_FUNC void
hive_offset_init(hive_offset *ho, Hive *hv, offset ofs)
{
    ho->hv = hv;
    offset_copy(&ho->ofs, ofs);
}
