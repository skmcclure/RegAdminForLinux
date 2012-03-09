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

#ifndef RREGADMIN_SAM_USERFINFO_H
#define RREGADMIN_SAM_USERFINFO_H 1

#include <rregadmin/util/macros.h>
#include <rregadmin/util/types.h>
#include <rregadmin/util/timestamp.h>
#include <rregadmin/registry/types.h>
#include <rregadmin/sam/types.h>

/**
 * @defgroup userfinfo_sam_group F Key User Info.
 *
 * @ingroup sam_group
 */


G_BEGIN_DECLS

typedef struct UserFInfo_ UserFInfo;

/** Create a new UserFInfo object for a ValueCell.
 *
 * @ingroup userfinfo_sam_group
 */
UserFInfo* userfinfo_new(ValueCell *in_cell);

/** Free a UserFInfo object.
 *
 * @ingroup userfinfo_sam_group
 */
gboolean userfinfo_free(UserFInfo *in_info);

/** Check the validity of a UserFInfo object.
 *
 * @ingroup userfinfo_sam_group
 */
gboolean userfinfo_is_valid(const UserFInfo *in_info);

/** Get the time of the last login.
 *
 * @ingroup userfinfo_sam_group
 */
NTTIME userfinfo_get_last_login(const UserFInfo *in_info);

/** Get the time of the last password change.
 *
 * @ingroup userfinfo_sam_group
 */
NTTIME userfinfo_get_last_password_change(const UserFInfo *in_info);

/** Get the time the account expires.
 *
 * @ingroup userfinfo_sam_group
 */
NTTIME userfinfo_get_account_expires(const UserFInfo *in_info);

/** Get the time of the entry of the last invalid password.
 *
 * @ingroup userfinfo_sam_group
 */
NTTIME userfinfo_get_last_invalid_password(const UserFInfo *in_info);

/** Get the rid.
 *
 * @ingroup userfinfo_sam_group
 */
rid_type userfinfo_get_rid(const UserFInfo *in_info);

/** Get the acb flags.
 *
 * @ingroup userfinfo_sam_group
 */
acb_type userfinfo_get_acb(const UserFInfo *in_info);

/** Get a string representing the acb flags.
 *
 * @ingroup userfinfo_sam_group
 */
const ustring* userfinfo_get_acb_string(const UserFInfo *in_info);

/** Get the bad login count.
 *
 * @ingroup userfinfo_sam_group
 */
int userfinfo_get_bad_login_count(const UserFInfo *in_info);

/** Get the number of logins.
 *
 * @ingroup userfinfo_sam_group
 */
int userfinfo_get_number_of_logins(const UserFInfo *in_info);

/** Get the country code.
 *
 * @ingroup userfinfo_sam_group
 */
country_code_type userfinfo_get_country_code(const UserFInfo *in_info);

/** Get debug output on the UserFInfo.
 *
 * @internal
 *
 * @ingroup uservinfo_sam_group
 */
gboolean userfinfo_get_debug_output(const UserFInfo *in_info, ustring *ustr);

/** Output UserFInfo in a debug format.
 *
 * @internal
 *
 * @ingroup userfinfo_sam_group
 */
void userfinfo_debug_print(const UserFInfo *in_info);

G_END_DECLS

#endif /* RREGADMIN_SAM_USERFINFO_H */
