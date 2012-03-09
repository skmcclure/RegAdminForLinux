/*
 * Authors:	James LewisMoss <jlm@racemi.com>
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

G_INLINE_FUNC guint32
offset_to_begin(offset in_ofs)
{
#ifdef RRA_OFS_USE_AGGREGATE
    return in_ofs.ofs;
#else
    union
    {
        guint64 i;
        struct offset_ o;
    } ob;

    ob.i = in_ofs;

    return ob.o.ofs;
#endif
}

G_INLINE_FUNC guint8*
offset_holder_get_data(offset_holder* oh, offset in_ofs)
{
    return oh->data + offset_to_begin(in_ofs);
}

G_INLINE_FUNC offset_holder*
offset_get_holder(offset in_ofs)
{
#ifdef RRA_OFS_USE_AGGREGATE
    return in_ofs.holder;
#else
    union
    {
        guint64 i;
        struct offset_ o;
    } ob;

    ob.i = in_ofs;

    return ob.o.holder;
#endif
}

G_INLINE_FUNC guint32
offset_to_end(offset ofs)
{
    return offset_holder_size(offset_get_holder(ofs)) - offset_to_begin(ofs);
}

G_INLINE_FUNC guint8*
offset_get_data(offset in_ofs)
{
    return offset_holder_get_data(offset_get_holder(in_ofs), in_ofs);
}

G_INLINE_FUNC gboolean
offset_copy (offset *in_dest, offset in_source)
{
#ifdef RRA_OFS_USE_AGGREGATE
    in_dest->holder = in_source.holder;
    in_dest->ofs = in_source.ofs;
#else
    *in_dest = in_source;
#endif
    return TRUE;
}

G_INLINE_FUNC gboolean
offset_make_invalid (offset *in_ofs)
{
    *in_ofs = INVALID_OFFSET;
    return TRUE;
}
