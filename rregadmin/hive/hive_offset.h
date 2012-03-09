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

#ifndef RREGADMIN_HIVE_HIVE_OFFSET_H
#define RREGADMIN_HIVE_HIVE_OFFSET_H 1

#include <glib/gutils.h>

#include <rregadmin/util/macros.h>
#include <rregadmin/util/offsets.h>
#include <rregadmin/hive/types.h>

G_BEGIN_DECLS

/**
 * @defgroup hive_offset_hive_group Hive Offset Object
 *
 * This object represents an offset into the hive.  All cells and bins
 * use this to contain their positions within the hive.
 *
 * @internal
 *
 * @ingroup hive_group
 */

/** Hive Offset Object.
 *
 * @ingroup hive_offset_hive_group
 */
struct hive_offset_
{
    offset ofs;
    Hive *hv;
};
typedef struct hive_offset_ hive_offset;

/** Get the hive from the hive_offset_
 *
 * @ingroup hive_offset_hive_group
 */
Hive* hive_offset_get_hive(hive_offset *ho);

/** Get the offset from the hive_offset_
 *
 * @ingroup hive_offset_hive_group
 */
offset hive_offset_get_offset(hive_offset *ho);

/** Initialize a hive_offset_
 *
 * @ingroup hive_offset_hive_group
 */
void hive_offset_init(hive_offset *ho, Hive *hv, offset ofs);

#if defined (G_CAN_INLINE)
#include <rregadmin/hive/hive_offset.inl>
#endif

G_END_DECLS

#endif // RREGADMIN_HIVE_HIVE_OFFSET_H
