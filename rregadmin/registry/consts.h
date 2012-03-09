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

#ifndef RREGADMIN_REGISTRY_CONSTS_H
#define RREGADMIN_REGISTRY_CONSTS_H 1

#include <rregadmin/util/macros.h>

/**
 * @defgroup registry_consts_group Registry Constants
 *
 * @ingroup registry_group
 */

G_BEGIN_DECLS

/** Location to top of SAM keys.
 *
 * @ingroup registry_consts_group
 */
#define SAM_KEY_NAME "\\HKEY_LOCAL_MACHINE\\SAM"

/** Location to top of system keys.
 *
 * @ingroup registry_consts_group
 */
#define SYSTEM_KEY_NAME "\\HKEY_LOCAL_MACHINE\\system"

/** Location to top of security keys.
 *
 * @ingroup registry_consts_group
 */
#define SECURITY_KEY_NAME "\\HKEY_LOCAL_MACHINE\\SECURITY"

/** Location to top of software keys.
 *
 * @ingroup registry_consts_group
 */
#define SOFTWARE_KEY_NAME "\\HKEY_LOCAL_MACHINE\\software"

/** Location to current user key.
 *
 * @ingroup registry_consts_group
 */
#define CURRENT_USER_KEY_NAME "\\HKEY_CURRENT_USER"

/** Location to default user key.
 *
 * @ingroup registry_consts_group
 */
#define DEFAULT_USER_KEY_NAME "\\HKEY_USERS\\DEFAULT"

/** Location to SID/RID Keys.
 *
 * @ingroup registry_consts_group
 */
#define SID_RID_CLASSES_USER_KEY_NAME "\\HKEY_USERS\\SID-RID_Classes"

/** Location to userdiff keys.
 *
 * @ingroup registry_consts_group
 */
#define USERDIFF_KEY_NAME "\\NonStandard\\USERDIFF"

/** Location to unknown key.
 *
 * @ingroup registry_consts_group
 */
#define UNKNOWN_KEY_NAME "\\NonStandard\\UNKNOWN"

G_END_DECLS

#endif // RREGADMIN_REGISTRY_CONSTS_H
