/*
 * Authors:       James LewisMoss <jlm@racemi.com>
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

#ifndef RREGADMIN_SAM_USER_H
#define RREGADMIN_SAM_USER_H 1

#include <rregadmin/registry/types.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/util/types.h>
#include <rregadmin/sam/types.h>
#include <rregadmin/secdesc/security_descriptor.h>

/**
 * @defgroup user_sam_group SamUser Object
 *
 * @ingroup sam_group
 */

G_BEGIN_DECLS

/** Create a new SamUser object for a particular registry and id.
 *
 * @ingroup samuser_sam_group
 */
SamUser* samuser_new_by_id(SamInfo *in_si, rid_type in_id);

/** Create a new SamUser object for a particular registry and name.
 *
 * @ingroup samuser_sam_group
 */
SamUser* samuser_new_by_name(SamInfo *in_si, const ustring *in_name);

/** Free a SamUser object.
 *
 * @ingroup samuser_sam_group
 */
gboolean samuser_free(SamUser *in_user);

/** Check the validity of a SamUser object.
 *
 * @ingroup samuser_sam_group
 */
gboolean samuser_is_valid(const SamUser *in_user);

/** Get the rid for the user.
 *
 * @ingroup samuser_sam_group
 */
rid_type samuser_get_rid(const SamUser *in_user);

/** Get the name for the user.
 *
 * @ingroup samuser_sam_group
 */
const ustring* samuser_get_username(const SamUser *in_user);

/** Get the fullname.
 *
 * @ingroup samuser_sam_group
 */
const ustring* samuser_get_fullname(const SamUser *in_user);

/** Get the comment.
 *
 * @ingroup samuser_sam_group
 */
const ustring* samuser_get_comment(const SamUser *in_user);

/** Get the user comment.
 *
 * @ingroup samuser_sam_group
 */
const ustring* samuser_get_user_comment(const SamUser *in_user);

/** Get the home directory.
 *
 * @ingroup samuser_sam_group
 */
const ustring* samuser_get_homedir(const SamUser *in_user);

/** Get the scriptpath.
 *
 * @ingroup samuser_sam_group
 */
const ustring* samuser_get_scriptpath(const SamUser *in_user);

/** Get the profilepath.
 *
 * @ingroup samuser_sam_group
 */
const ustring* samuser_get_profilepath(const SamUser *in_user);

/** Get the security descriptor.
 *
 * @ingroup samuser_sam_group
 */
const SecurityDescriptor* samuser_get_secdesc(const SamUser *in_user);

/** Get the time of the last login.
 *
 * @ingroup samuser_sam_group
 */
NTTIME samuser_get_last_login(const SamUser *in_user);

/** Get the time of the last password change.
 *
 * @ingroup samuser_sam_group
 */
NTTIME samuser_get_last_password_change(const SamUser *in_user);

/** Get the time the account expires.
 *
 * @ingroup samuser_sam_group
 */
NTTIME samuser_get_account_expires(const SamUser *in_user);

/** Get the time of the entry of the last invalid password.
 *
 * @ingroup samuser_sam_group
 */
NTTIME samuser_get_last_invalid_password(const SamUser *in_user);

/** Get the acb flags.
 *
 * @ingroup samuser_sam_group
 */
acb_type samuser_get_acb(const SamUser *in_user);

/** Get a string representing the acb flags.
 *
 * @ingroup samuser_sam_group
 */
const ustring* samuser_get_acb_string(const SamUser *in_user);

/** Get the bad login count.
 *
 * @ingroup samuser_sam_group
 */
int samuser_get_bad_login_count(const SamUser *in_user);

/** Get the number of logins.
 *
 * @ingroup samuser_sam_group
 */
int samuser_get_number_of_logins(const SamUser *in_user);

/** Get the country code.
 *
 * @ingroup samuser_sam_group
 */
country_code_type samuser_get_country_code(const SamUser *in_user);

/** Print debug info on a SamUser object.
 *
 * @ingroup samuser_sam_group
 */
void samuser_debug_print(const SamUser *in_user);

/** Get debugging output.
 *
 * @internal
 *
 * @ingroup samuser_sam_group
 */
gboolean samuser_get_debug_output(const SamUser *in_user, ustring *in_ustr);

G_END_DECLS

#endif /* RREGADMIN_SAM_USER_H */
