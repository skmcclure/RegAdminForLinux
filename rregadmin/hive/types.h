/*
 * Structs that represent the format of data stored in registry hives and
 * related functions
 *
 * Authors:     Sean Loaring
 *              Petter Nordahl-Hagen
 *              James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi Inc
 * Copyright (c) 2005-2006 Sean Loaring
 * Copyright (c) 1997-2004 Petter Nordahl-Hagen
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

#ifndef RREGADMIN_HIVE_TYPES_H
#define RREGADMIN_HIVE_TYPES_H

#include <glib/gtypes.h>

#include <rregadmin/util/macros.h>
#include <rregadmin/util/timestamp.h>

/*
 * This file contains some declarations for data structures.
 *
 * It was easier to just declare them here rather than try to move the
 * declarations to appropriate header files because there is some
 * circularity in dependencies.
 */
G_BEGIN_DECLS

/** Structure containing hive version information.
 *
 * @ingroup hive_hive_group
 */
struct HiveVersion_
{
    /** Major version number.
     *
     * I've only encountered version 1.
     */
    guint32 major;
    /** Minor version number.
     *
     * I've only encountered versions 3 and 5.
     */
    guint32 minor;
    /** Release number.
     *
     * I've only encounterd version 0.
     */
    guint32 release;
    /** Build number
     */
    guint32 build;
} RRA_VERBATIM_STRUCT;
typedef struct HiveVersion_ HiveVersion;

typedef struct Hive_ Hive;

typedef struct Bin_ Bin;

typedef struct Cell_ Cell;

/** Statistics for a Hive_ or Bin_.
 */
struct used_stats
{
    /** Number of blocks.
     */
    guint32 blocks;

    /** Number of keys.
     */
    guint32 keys;

    /** Number of values.
     */
    guint32 values;

    /** Bytes used.
     */
    guint32 used;
    /** Bytes unused.
     */
    guint32 free;
    /** Bytes used for meta-data (headers and such).
     */
    guint32 data;
    /** Bytes that seem unallocated.
     */
    guint32 lost;
};

typedef struct SecurityDescriptorCell_ SecurityDescriptorCell;

typedef struct KeyCell_ KeyCell;

typedef struct ValueKeyCell_ ValueKeyCell;

typedef struct KeyListCell_ KeyListCell;

typedef struct ValueListCell_ ValueListCell;

typedef struct ValueCell_ ValueCell;

G_END_DECLS

#endif // RREGADMIN_HIVE_TYPES_H
