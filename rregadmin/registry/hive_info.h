/*
 *
 * Authors:     Sean Loaring
 *              James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi Inc
 * Copyright (c) 2005-2006 Sean Loaring
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

#ifndef RREGADMIN_REGISTRY_HIVE_INFO_H
#define RREGADMIN_REGISTRY_HIVE_INFO_H 1

#include <rregadmin/registry/types.h>
#include <rregadmin/hive/types.h>
#include <rregadmin/util/path.h>
#include <rregadmin/util/macros.h>


/**
 * @defgroup hive_info_registry_group hive_info Object
 *
 * This object contains information and behavior for loaded hives.
 *
 * @ingroup registry_group
 */

G_BEGIN_DECLS

/** Type representing a hive type
 *
 * @ingroup hive_info_registry_group
 */
typedef enum
{
    /** Unknown hive type.
     *
     * @ingroup hive_info_registry_group
     */
    HTYPE_UNKNOWN = 0,

    /** Sam hive type.
     *
     * @ingroup hive_info_registry_group
     */
    HTYPE_SAM = 1,

    /** System hive type.
     *
     * @ingroup hive_info_registry_group
     */
    HTYPE_SYSTEM = 2,

    /** Security hive type.
     *
     * @ingroup hive_info_registry_group
     */
    HTYPE_SECURITY = 3,

    /** Software hive type.
     *
     * @ingroup hive_info_registry_group
     */
    HTYPE_SOFTWARE = 4,

    /** User hive type.
     *
     * @ingroup hive_info_registry_group
     */
    HTYPE_USER = 5,

    /** User diff hive type.
     *
     * @ingroup hive_info_registry_group
     */
    HTYPE_USERDIFF = 6,

    /** User class hive type.
     *
     * @ingroup hive_info_registry_group
     */
    HTYPE_USRCLASS = 7,

    /** Default hive type.
     *
     * @ingroup hive_info_registry_group
     */
    HTYPE_DEFAULT = 8,

    /** Unloaded hive type.
     *
     * @ingroup hive_info_registry_group
     */
    HTYPE_UNLOADED = -1,

    /** Error hive type.
     *
     * @ingroup hive_info_registry_group
     */
    HTYPE_ERROR = -2,
} HiveType;

/** Create a new hive_info.
 *
 * @ingroup hive_info_registry_group
 */
hive_info* hive_info_new(const char *hive_filename) G_GNUC_WARN_UNUSED_RESULT;

/** Free a hive_info.
 *
 * @ingroup hive_info_registry_group
 */
gboolean hive_info_free (hive_info *in_hi);

/** Write the hive out if it's dirty.
 *
 * @ingroup hive_info_registry_group
 */
gboolean hive_info_flush(hive_info *in_hi, gboolean in_do_backup);

/** Unload the hive info.
 *
 * @ingroup hive_info_registry_group
 */
gboolean hive_info_unload(hive_info *in_hi, gboolean in_do_backup);

/** Check hive validity.
 *
 * @ingroup hive_info_registry_group
 */
gboolean hive_info_is_valid(hive_info *in_hi);

/** Is the hive dirty?
 *
 * @ingroup hive_info_registry_group
 */
gboolean hive_info_is_dirty(const hive_info *in_hi);

/** Get the hive.
 *
 * @ingroup hive_info_registry_group
 */
Hive* hive_info_get_hive(const hive_info *in_hi);

/** Get the hive filename.
 *
 * @ingroup hive_info_registry_group
 */
const char* hive_info_get_filename(const hive_info *in_hi);

/** Get the hive type.
 *
 * @ingroup hive_info_registry_group
 */
HiveType hive_info_get_type(const hive_info *in_hi);

/** Get the hive id.
 *
 * @ingroup hive_info_registry_group
 */
hive_id hive_info_get_id(const hive_info *in_hi);

/** Set the hive id.
 *
 * @ingroup hive_info_registry_group
 */
void hive_info_set_id(hive_info *in_hi, hive_id in_id);

/** Guess the type of the hive.
 *
 * @internal
 *
 * @ingroup hive_info_registry_group
 */
HiveType guess_hive_type (Hive *hdesc);

G_END_DECLS

#endif // RREGADMIN_REGISTRY_HIVE_INFO_H
