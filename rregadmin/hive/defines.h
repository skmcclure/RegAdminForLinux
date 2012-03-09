/*
 * Defines identification strings for different structures stored in the
 * registry.  Also defines files access modes.
 *
 * Authors:	Sean Loaring
 *		Petter Nordahl-Hagen
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
 * TODO: this file should be hacked up and the bits placed in other
 * header files
 *
 */

#ifndef RREGADMIN_HIVE_DEFINES_H
#define RREGADMIN_HIVE_DEFINES_H 1

#include <rregadmin/util/macros.h>

G_BEGIN_DECLS

/** The length of a hive page.
 *
 * All hives are a multiple of this size.
 *
 * A Bin_ is at least this large and if larger is a multiple of this size.
 *
 * @ingroup hive_group
 */
#define HIVE_PAGE_LEN 0x1000

/** Magic number at the begining of hives.
 *
 * @ingroup hive_group
 */
#define HIVE_MAGIC 0x66676572

typedef enum
{
    /** Id for a SecurityDescriptorCell_
     *
     * @ingroup cell_hive_group
     */
    ID_SK_KEY = 0x6b73,

    /** One of the ids for a KeyListCell_.
     *
     * @ingroup cell_hive_group
     */
    ID_LF_KEY = 0x666c,

    /** One of the ids for a KeyListCell_.
     *
     * @ingroup cell_hive_group
     */
    ID_LH_KEY = 0x686c,

    /** One of the ids for a KeyListCell_.
     *
     * @ingroup cell_hive_group
     */
    ID_LI_KEY = 0x696c,

    /** One of the ids for a KeyListCell_.
     *
     * @ingroup cell_hive_group
     */
    ID_RI_KEY = 0x6972,

    /** Id for a ValueKeyCell_.
     *
     * @ingroup cell_hive_group
     */
    ID_VK_KEY = 0x6b76,

    /** Id for a KeyCell_.
     *
     * @ingroup cell_hive_group
     */
    ID_NK_KEY = 0x6b6e,
} CellID;

/** Hive mode bits.
 */
typedef enum
{
    HMODE_RW = 0,
    HMODE_RO = 0x1,
    HMODE_OPEN = 0x2,
    HMODE_DIRTY = 0x4,
    HMODE_NOALLOC = 0x8,
    HMODE_VERBOSE = 0x1000,
} HiveMode;

G_END_DECLS

#endif /* RREGADMIN_HIVE_DEFINES_H */

// vim600: set foldlevel=0 foldmethod=marker:
