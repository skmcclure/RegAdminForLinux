/*
 * Authors:     Sean Loaring
 *              Petter Nordahl-Hagen
 *              James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2007 Racemi Inc
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

#ifndef RREGADMIN_SAM_USERVINFO_H
#define RREGADMIN_SAM_USERVINFO_H 1

#include <rregadmin/registry/types.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/secdesc/security_descriptor.h>

/**
 * @defgroup uservinfo_sam_group V Key User Info.
 *
 * @ingroup sam_group
 */

G_BEGIN_DECLS

typedef struct UserVInfo_ UserVInfo;

/** Create a new UserVInfo object for a ValueCell.
 *
 * @ingroup uservinfo_sam_group
 */
UserVInfo* uservinfo_new(ValueCell *in_cell);

/** Free a UserVInfo object.
 *
 * @ingroup uservinfo_sam_group
 */
gboolean uservinfo_free(UserVInfo *in_info);

/** Check the validity of a UserVInfo object.
 *
 * @ingroup uservinfo_sam_group
 */
gboolean uservinfo_is_valid(const UserVInfo *in_info);

/** Get the security descriptor.
 *
 * @ingroup uservinfo_sam_group
 */
const SecurityDescriptor* uservinfo_get_secdesc(const UserVInfo *in_info);

/** Get the username.
 *
 * @ingroup uservinfo_sam_group
 */
const ustring* uservinfo_get_username(const UserVInfo *in_info);

/** Get the fullname.
 *
 * @ingroup uservinfo_sam_group
 */
const ustring* uservinfo_get_fullname(const UserVInfo *in_info);

/** Get the comment.
 *
 * @ingroup uservinfo_sam_group
 */
const ustring* uservinfo_get_comment(const UserVInfo *in_info);

/** Get the user comment.
 *
 * @ingroup uservinfo_sam_group
 */
const ustring* uservinfo_get_user_comment(const UserVInfo *in_info);

/** Get the home directory.
 *
 * @ingroup uservinfo_sam_group
 */
const ustring* uservinfo_get_homedir(const UserVInfo *in_info);

/** Get the scriptpath.
 *
 * @ingroup uservinfo_sam_group
 */
const ustring* uservinfo_get_scriptpath(const UserVInfo *in_info);

/** Get the profilepath.
 *
 * @ingroup uservinfo_sam_group
 */
const ustring* uservinfo_get_profilepath(const UserVInfo *in_info);

/** Get debug output on the UserVInfo.
 *
 * @internal
 *
 * @ingroup uservinfo_sam_group
 */
gboolean uservinfo_get_debug_output(const UserVInfo *in_info, ustring *ustr);

/** Print debugging output for the UserVInfo.
 *
 * @internal
 *
 * @ingroup uservinfo_sam_group
 */
void uservinfo_debug_print(const UserVInfo *in_info);

G_END_DECLS

#endif /* RREGADMIN_SAM_USERVINFO_H */
