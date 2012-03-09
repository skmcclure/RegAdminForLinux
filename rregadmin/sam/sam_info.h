/*
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

#ifndef RREGADMIN_SAM_SAM_INFO_H
#define RREGADMIN_SAM_SAM_INFO_H 1

#include <rregadmin/registry/types.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/util/timestamp.h>
#include <rregadmin/util/ustring_list.h>
#include <rregadmin/util/types.h>
#include <rregadmin/sam/types.h>

/**
 * @defgroup sam_info_sam_group SamInfo Object
 *
 * @ingroup sam_group
 */

G_BEGIN_DECLS

/** Create a new SamInfo object for a particular registry.
 *
 * @ingroup sam_info_sam_group
 */
SamInfo* sam_info_new(Registry *in_reg);

/** Free a SamInfo object.
 *
 * @ingroup sam_info_sam_group
 */
gboolean sam_info_free(SamInfo *in_info);

/** Check the validity of a SamInfo object.
 *
 * @ingroup sam_info_sam_group
 */
gboolean sam_info_is_valid(const SamInfo *in_info);


/** Get next rid.
 *
 * @ingroup sam_info_sam_group
 */
gint32 sam_info_get_next_rid(const SamInfo *in_info);

/** Get auto increment.
 *
 * @ingroup sam_info_sam_group
 */
guint32 sam_info_get_auto_increment(const SamInfo *in_info);

/** Get the maximum password age.
 *
 * @ingroup sam_info_sam_group
 */
NTTIME sam_info_get_max_password_age(const SamInfo *in_info);

/** Get the minimum password age.
 *
 * @ingroup sam_info_sam_group
 */
NTTIME sam_info_get_min_password_age(const SamInfo *in_info);

/** Get the lockout duration.
 *
 * @ingroup sam_info_sam_group
 */
NTTIME sam_info_get_lockout_duration(const SamInfo *in_info);

/** Get the lockout reset time.
 *
 * @ingroup sam_info_sam_group
 */
NTTIME sam_info_get_lockout_reset(const SamInfo *in_info);

/** Get the sam fkey flags.
 *
 * @ingroup sam_info_sam_group
 */
guint32 sam_info_get_flags(const SamInfo *in_info);

/** Get the min pass.
 *
 * @ingroup sam_info_sam_group
 */
guint16 sam_info_get_min_pass(const SamInfo *in_info);

/** Get whether password history should be enforced.
 *
 * @ingroup sam_info_sam_group
 */
gboolean sam_info_should_enforce_password_history(const SamInfo *in_info);

/** Get the lockout threshold.
 *
 * @ingroup sam_info_sam_group
 */
guint16 sam_info_get_lockout_threshold(const SamInfo *in_info);

/** Get the obfuscated syskey.
 *
 * @ingroup sam_info_sam_group
 */
guint8* sam_info_get_obfs_syskey(const SamInfo *in_info);


/** Get the number of users.
 *
 * @ingroup sam_info_sam_group
 */
int sam_info_get_number_of_users(const SamInfo *in_info);

/** Get list of user names.
 *
 * @ingroup sam_info_sam_group
 */
const ustring_list* sam_info_get_user_name_list (const SamInfo *in_info);

/** Get list of user rids.
 *
 * @ingroup sam_info_sam_group
 */
const GArray* sam_info_get_user_rid_list (const SamInfo *in_info);

/** Get user by rid.
 *
 * @ingroup sam_info_sam_group
 */
SamUser* sam_info_get_user_by_id(SamInfo *in_info, rid_type in_id);

/** Get user by name
 *
 * @ingroup sam_info_sam_group
 */
SamUser* sam_info_get_user_by_name(SamInfo *in_info, const ustring *in_name);

/** Get the registry associated with this SamInfo.
 *
 * @internal
 *
 * @ingroup sam_info_sam_group
 */
Registry* sam_info_get_registry(SamInfo *in_info);

/** Print debug info on a SamInfo object.
 *
 * @internal
 *
 * @ingroup sam_info_sam_group
 */
void sam_info_debug_print(const SamInfo *in_info);

G_END_DECLS

#endif /* RREGADMIN_SAM_SAM_INFO_H */
