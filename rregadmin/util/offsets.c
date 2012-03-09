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
#define G_IMPLEMENT_INLINES
#include <rregadmin/config.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/util/offsets.h>
#include <rregadmin/util/malloc.h>
#include <rregadmin/util/log.h>

static const guint32 HOLDER_MAGIC = 0x08877880;
static const guint32 OFFSET_MAGIC = 0x07788770;

#if 0

#define CHECK_OH_RETURN(oh, ret)                \
    if (!offset_holder_is_valid((oh)))          \
        return (ret)

#define CHECK_O_RETURN(ofs, ret)                \
    if (!offset_is_valid((ofs)))                \
        return (ret)
#else

#define CHECK_OH_RETURN(oh, ret)
#define CHECK_O_RETURN(ofs, ret)

#endif

static offset offset_new(offset_holder *oh, guint32 in_ofs);

offset_holder *
offset_holder_new(guint8 *in_buf, guint32 in_size)
{
    offset_holder *ret_val = g_new0(offset_holder, 1);
    ret_val->magic = HOLDER_MAGIC;
    ret_val->buf = in_buf;
    ret_val->data = ret_val->buf;
    ret_val->header_size = 0;
    ret_val->used_size = in_size;
    ret_val->state = 0;
    return ret_val;
}

offset_holder*
offset_holder_new_with_header(guint8 *in_buf, guint32 in_header_size,
                              guint32 in_size)
{
    offset_holder *ret_val = offset_holder_new(in_buf, in_size);
    ret_val->data = ret_val->buf + in_header_size;
    ret_val->header_size = in_header_size;
    return ret_val;
}

gboolean
offset_holder_free(offset_holder *oh)
{
    if (offset_holder_is_valid(oh))
    {
        if (oh->buf != NULL)
        {
            g_free(oh->buf);
            oh->buf = 0;
        }
        oh->data = 0;
        oh->used_size = 0;
        oh->header_size = 0;
        oh->magic = 0;
        g_free(oh);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

gboolean
offset_holder_resize(offset_holder *in_oh, gint32 in_change)
{
    CHECK_OH_RETURN(in_oh, FALSE);

    if ((gint32)offset_holder_size(in_oh) + in_change < 0)
    {
        rra_message(N_("Tried to reduce size below 0"));
        return FALSE;
    }

    if (in_change == 0)
    {
        return TRUE;
    }

    guint8 *new_buf = g_renew(guint8, in_oh->buf,
                              in_oh->used_size + in_change);
    if (new_buf != NULL)
    {
        in_oh->buf = new_buf;
        in_oh->data = in_oh->buf + in_oh->header_size;
        if (in_change > 0)
        {
            memset(in_oh->data + offset_holder_size(in_oh), 0, in_change);
        }
        in_oh->used_size += in_change;
        return TRUE;
    }
    else
    {
        rra_warning(N_("g_renew(guint8, ptr, %d) failed"),
                    in_oh->used_size + in_change);
        return FALSE;
    }
}

gboolean
offset_holder_truncate(offset_holder *in_oh, guint32 in_change)
{
    return offset_holder_resize(in_oh, -in_change);
}

offset
offset_holder_grow(offset_holder *oh, guint32 in_extra)
{
    guint32 old_max_offset = offset_holder_size(oh);

    if (offset_holder_resize(oh, in_extra))
    {
        return offset_holder_make_offset(oh, old_max_offset);
    }
    else
    {
        return INVALID_OFFSET;
    }
}

gboolean
offset_holder_is_valid(const offset_holder *oh)
{
    if (oh->magic != HOLDER_MAGIC)
    {
        return FALSE;
    }

    return TRUE;
}

guint32
offset_holder_header_size(const offset_holder *oh)
{
    CHECK_OH_RETURN(oh, 0);
    return oh->header_size;
}

guint32
offset_holder_size(const offset_holder *oh)
{
    CHECK_OH_RETURN(oh, 0);
    return oh->used_size - oh->header_size;
}

guint8*
offset_holder_get_header(offset_holder *oh)
{
    CHECK_OH_RETURN(oh, NULL);
    return oh->buf;
}

guint32
offset_holder_buf_size(const offset_holder *oh)
{
    CHECK_OH_RETURN(oh, 0);
    return oh->used_size;
}

guint8*
offset_holder_get_buf(offset_holder *oh)
{
    CHECK_OH_RETURN(oh, NULL);
    return oh->buf;
}

gboolean
offset_holder_is_valid_offset(offset_holder *oh, guint32 in_ofs)
{
    if (in_ofs >= offset_holder_size(oh))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static gboolean
offset_init (offset *in_init, offset_holder *in_oh, guint32 in_ofs)
{
    if (!offset_holder_is_valid_offset(in_oh, in_ofs))
    {
        rra_info(N_("Trying to init oh (size: %d) for invalid offset: %d"),
                 offset_holder_size(in_oh), in_ofs);
        return FALSE;
    }

    ((struct offset_*)in_init)->ofs = in_ofs;
    ((struct offset_*)in_init)->holder = in_oh;
    return TRUE;
}

static offset
offset_new(offset_holder *oh, guint32 in_ofs)
{
    offset ret_val;
    if (offset_init(&ret_val, oh, in_ofs))
    {
        return ret_val;
    }
    else
    {
        return INVALID_OFFSET;
    }
}

offset
offset_holder_make_offset(offset_holder *oh, guint32 in_ofs)
{
    return offset_new(oh, in_ofs);
}

gboolean
offset_holder_init_offset (offset *in_init, offset_holder *oh, guint32 in_ofs)
{
    return offset_init(in_init, oh, in_ofs);
}

offset
offset_make_relative(offset ofs, gint32 in_rel_ofs)
{
    return offset_holder_make_offset(offset_get_holder(ofs),
                                     offset_to_begin(ofs) + in_rel_ofs);
}

gboolean
offset_init_relative (offset *in_init, offset in_ofs,
                      gint32 in_rel_ofs)
{
    return offset_init(in_init, offset_get_holder(in_ofs),
                       offset_to_begin(in_ofs) + in_rel_ofs);
}

gboolean
offset_is_valid(const offset in_ofs)
{
    static offset invalid_offset = INVALID_OFFSET;
    if (in_ofs.ofs == invalid_offset.ofs)
    {
        return FALSE;
    }
    if (in_ofs.holder == invalid_offset.holder)
    {
        return FALSE;
    }

    if (offset_get_holder(in_ofs) == NULL)
    {
        return FALSE;
    }

    if (offset_to_begin(in_ofs)
        > offset_holder_size(offset_get_holder(in_ofs)))
    {
        return FALSE;
    }

    return TRUE;
}

#define G_INLINE_FUNC
#include <rregadmin/util/offsets.inl>

gint32
offset_diff(offset o1, offset o2)
{
    return offset_to_begin(o2) - offset_to_begin(o1);
}

gboolean
offset_equal(offset a, offset b)
{
    return offset_to_begin(a) == offset_to_begin(b);
}

gint
offset_compare(offset a, offset b)
{
    return offset_to_begin(a) - offset_to_begin(b);
}

void
offset_debug_print(offset ofs)
{
    fprintf (stderr, "<offset ");
    fprintf (stderr, "offset=\"%#x\", ", offset_to_begin(ofs));
    fprintf (stderr, "holder=\"%p\"/>\n", offset_get_holder(ofs));
}

void
offset_holder_debug_print(const offset_holder *oh,
                          gboolean DECLARE_UNUSED(in_verbose))
{
    fprintf (stderr, "<offset_holder>\n");
    fprintf (stderr, "  <header_size value=\"%#x\"/>\n", oh->header_size);
    fprintf (stderr, "  <used_size value=\"%#x\"/>\n", oh->used_size);
    fprintf (stderr, "</offset_holder>\n");
}
