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
/**
 * @file   rregadmin/util/offsets.h
 * @author James Lewismoss <jlm@racemi.com>
 * @date   Thu Mar 27 16:43:37 2008
 *
 * @brief  Functions and structs to handle offsets.
 *
 *
 */

#ifndef RREGADMIN_UTIL_OFFSETS_H
#define RREGADMIN_UTIL_OFFSETS_H 1

#include <glib/gtypes.h>
#include <glib/garray.h>
#include <glib/gutils.h>

#include <rregadmin/util/macros.h>

/**
 * @defgroup offset_holder_object Offset and Offset Holder Objects
 *
 * @ingroup util_group
 */
G_BEGIN_DECLS

typedef struct offset_holder_ offset_holder;

#define RRA_OFS_USE_AGGREGATE 1

#ifdef RRA_OFS_USE_AGGREGATE
typedef struct offset_ offset;
#else
typedef guint64 offset;
#endif

/** Create a new offset_holder_ using the provided buffer.
 *
 * This function takes ownership of the buffer passed in.  So do not
 * modify or delete it afterwards.
 *
 * @param in_buf the data
 * @param in_size the size of the data in bytes.
 * @return a new offset_holder_
 *
 * @ingroup offset_holder_object
 */
offset_holder* offset_holder_new(guint8 *in_buf, guint32 in_size);

/** Create a new offset_holder_ where a portion of the buffer is set aside
 * as a header.
 *
 * This is specifically designed for hives where the first block (4096 bytes)
 * is a header.  All offsets in the data of the hive are from after the header.
 *
 * This function takes ownership of the buffer passed in.  So do not
 * modify or delete it afterwards.
 *
 * @param in_buf the data
 * @param in_header_size the size of the header at the begining of the data.
 * @param in_size the size of the data in bytes.
 * @return a new offset_holder_
 *
 * @ingroup offset_holder_object
 */
offset_holder* offset_holder_new_with_header(guint8 *in_buf,
                                             guint32 in_header_size,
                                             guint32 in_size);

/** Free an offset_holder_.
 *
 * This is important to do because the offset_holder_ keeps a pointer
 * to all offset_s as well as the buffer and other misc data.  This is
 * how below all offset_s are constant.
 *
 * @ingroup offset_holder_object
 */
gboolean offset_holder_free(offset_holder *oh);

/**
 * Add in_extra space to the end of the offset holder's buffer.
 *
 * Increase the size of the buffer for the offset holder.
 *
 * If in_extra is zero then the return value will be NULL.
 *
 * @param oh the offset_holder_
 * @param in_extra the amount of space to add to the end of the buffer.
 * @return a new offset pointing to the new area in the buffer.
 *
 * @ingroup offset_holder_object
 */
offset offset_holder_grow(offset_holder *oh, guint32 in_extra);

/**
 * Chop in_less amount of bytes off the end of the offset holder's buffer.
 *
 * If in_less is greater than the size of the buffer no action is taken
 * and FALSE is returned.
 *
 * @param in_oh the offset_holder_
 * @param in_less the amount to remove from the end of the buffer.
 * @return whether the function succeeded.
 *
 * @ingroup offset_holder_object
 */
gboolean offset_holder_truncate(offset_holder *in_oh, guint32 in_less);

/** Resize the offset holder's buffer by in_change.
 *
 * Either shrinking or expanding the buffer from the end based on the
 * sign of in_change (positive == grow, negative == shrink).
 *
 * @param in_oh the offset_holder_
 * @param in_change the amount to change the buffer's size.
 * @return whether the function succeeded.
 *
 * @ingroup offset_holder_object
 */
gboolean offset_holder_resize(offset_holder *in_oh, gint32 in_change);

/** Check to see if the offset_holder_ is valid.
 *
 * This function is not often useful except during testing.
 *
 * @ingroup offset_holder_object
 */
gboolean offset_holder_is_valid(const offset_holder *oh);

/** Get the size of the offset_holder_'s header.
 *
 * @ingroup offset_holder_object
 */
guint32 offset_holder_header_size(const offset_holder *oh);

/** Get the size of the offset_holder_'s buffer (minus the size of the header)
 *
 * @ingroup offset_holder_object
 */
guint32 offset_holder_size(const offset_holder *oh);

/** Get a pointer to the header.
 *
 * This is neccessary since offset_s point into the data section not
 * the header.
 *
 * Be careful of this function.  Do not keep a copy of the return value
 * around because if the buffer is resized it may become invalid.
 *
 * @ingroup offset_holder_object
 */
guint8* offset_holder_get_header(offset_holder *oh);

/** Get the raw size of the buffer (including the header).
 *
 * @ingroup offset_holder_object
 */
guint32 offset_holder_buf_size(const offset_holder *oh);

/** Get pointer to the buffer. (including header)
 *
 * This is needed if you need to write out the buffer or somesuch.
 *
 * @ingroup offset_holder_object
 */
guint8* offset_holder_get_buf(offset_holder *oh);

/** Check to see if an offset into the data is valid.
 *
 * @ingroup offset_holder_object
 */
gboolean offset_holder_is_valid_offset(offset_holder *oh, guint32 in_ofs);

/** Create an offset_.
 *
 * @ingroup offset_holder_object
 */
offset offset_holder_make_offset(offset_holder *oh, guint32 in_ofs);

/** Initialize an offset.
 *
 * @ingroup offset_holder_object
 */
gboolean offset_holder_init_offset (offset *in_init, offset_holder *oh,
                                    guint32 in_ofs);

/** Create a new offset that is relative to the current one.
 *
 * @ingroup offset_holder_object
 */
offset offset_make_relative(const offset ofs, gint32 in_rel_ofs);

/** Initiliaze an offset relative to another one.
 *
 * @ingroup offset_holder_object
 */
gboolean offset_init_relative (offset *in_init, const offset in_ofs,
                               gint32 in_rel_ofs);

/** Copy offset to another.
 *
 * @ingroup offset_holder_object
 */
gboolean offset_copy (offset *in_dest, offset in_source);

/** Make an offset invalid.
 *
 * @ingroup offset_holder_object
 */
gboolean offset_make_invalid (offset *in_ofs);

/** Get the number of bytes between the offset and the begining of
 * the data.
 *
 * @ingroup offset_holder_object
 */
guint32 offset_to_begin(const offset in_ofs);

/** Get the number of bytes between the offset and the end of the data.
 *
 * @ingroup offset_holder_object
 */
guint32 offset_to_end(const offset ofs);

/** Check to see if an offset_ is valid.
 *
 * It is possible for an offset to become invalid.  If it points to a
 * part of the data that has been truncated.
 *
 * @ingroup offset_holder_object
 */
gboolean offset_is_valid(const offset in_ofs);

/** Get the data pointed to by this offset.
 *
 * Do not store this pointer.  Use it and forget it.
 *
 * @ingroup offset_holder_object
 */
guint8* offset_get_data(const offset in_ofs);

/** Get the offset_holder_ associated with this offset_.
 *
 * @ingroup offset_holder_object
 */
offset_holder* offset_get_holder(offset in_ofs);

/** Get the difference between two offset_s.
 *
 * @ingroup offset_holder_object
 */
gint32 offset_diff(offset o1, offset o2);

/** Compare offsets for equality.
 *
 * @ingroup offset_holder_object
 */
gboolean offset_equal(offset a, offset b);

/** Do a compare (like strcmp) on two offsets_s.
 *
 * @ingroup offset_holder_object
 */
gint offset_compare(offset a, offset b);

/** Print an xml description of the offset_.
 *
 * @internal
 *
 * @ingroup offset_holder_object
 */
void offset_debug_print(offset ofs);

/** Print an xml description of the offset_holder_.
 *
 * @internal
 *
 * @ingroup offset_holder_object
 */
void offset_holder_debug_print(const offset_holder *oh, gboolean in_verbose);

/** Get data from a specific offset.
 *
 * @internal
 *
 * @ingroup offset_holder_object
 */
guint8* offset_holder_get_data(offset_holder* oh, offset in_ofs);


#ifdef RRA_OFS_USE_AGGREGATE
#define INVALID_OFFSET (offset){ (guint32)-1, (offset_holder*)-1 }
#else
#define INVALID_OFFSET 0
#endif

/** Structure to hold a buffer of data and provide offsets into the data.
 *
 * This exists to allow the buffer to be resized thereby invalidating
 * any pointers into the buffer.  Instead everything accesses the buffer
 * using offsets.  Only when directly accessing the data do you obtain
 * a pointer into the buffer and only for a single operation.
 *
 * This object is not thread safe.
 *
 * @ingroup offset_holder_object
 */
struct offset_holder_
{
    /** Magic integer to allow validation
     */
    guint32 magic;
    /** The buffer containing the header and data.
     */
    guint8 *buf;
    /** Pointer into buf past the header.
     */
    guint8 *data;
    /** The size of the header.
     */
    guint32 header_size;
    /** The size of the buffer.
     */
    guint32 used_size;
    /** State data.
     */
    guint32 state;
};

/** An offset into a offset_holder_.
 *
 * Each offset_ is related to one offset_holder_ so there is no need to
 * pass the holder with the offset.
 *
 * @ingroup offset_holder_object
 */
struct offset_
{
    /** The offset into the data.
     */
    guint32 ofs;
    /** The offset holder with the data.
     */
    offset_holder *holder;
};

#if defined (G_CAN_INLINE)
#include <rregadmin/util/offsets.inl>
#endif

G_END_DECLS

#endif // RREGADMIN_UTIL_OFFSETS_H
