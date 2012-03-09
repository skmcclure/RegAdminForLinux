/*
  Copyright 2007 Racemi Inc
  Copyright (C) Andrew Tridgell 		2004

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef RREGADMIN_SECURITY_DESCRIPTOR_PRIVILEGE_H
#define RREGADMIN_SECURITY_DESCRIPTOR_PRIVILEGE_H

#include <glib.h>

#include <rregadmin/util/macros.h>

/**
 * @defgroup privilege_secdesc_group Privilege id handling code.
 *
 * @ingroup secdesc_group
 */

G_BEGIN_DECLS

typedef enum
{
    SEC_PRIV_SECURITY = 1,
    SEC_PRIV_BACKUP = 2,
    SEC_PRIV_RESTORE = 3,
    SEC_PRIV_SYSTEMTIME = 4,
    SEC_PRIV_SHUTDOWN = 5,
    SEC_PRIV_REMOTE_SHUTDOWN = 6,
    SEC_PRIV_TAKE_OWNERSHIP = 7,
    SEC_PRIV_DEBUG = 8,
    SEC_PRIV_SYSTEM_ENVIRONMENT = 9,
    SEC_PRIV_SYSTEM_PROFILE = 10,
    SEC_PRIV_PROFILE_SINGLE_PROCESS = 11,
    SEC_PRIV_INCREASE_BASE_PRIORITY = 12,
    SEC_PRIV_LOAD_DRIVER = 13,
    SEC_PRIV_CREATE_PAGEFILE = 14,
    SEC_PRIV_INCREASE_QUOTA = 15,
    SEC_PRIV_CHANGE_NOTIFY = 16,
    SEC_PRIV_UNDOCK = 17,
    SEC_PRIV_MANAGE_VOLUME = 18,
    SEC_PRIV_IMPERSONATE = 19,
    SEC_PRIV_CREATE_GLOBAL = 20,
    SEC_PRIV_ENABLE_DELEGATION = 21,
    SEC_PRIV_INTERACTIVE_LOGON = 22,
    SEC_PRIV_NETWORK_LOGON = 23,
    SEC_PRIV_REMOTE_INTERACTIVE_LOGON = 24
} sec_privilege;


/** map a privilege id to the wire string constant
 *
 * @ingroup privilege_secdesc_group
 */
const char *sec_privilege_name(sec_privilege privilege);

/** map a privilege id to a privilege display name. Return NULL if not found
 *
 * @ingroup privilege_secdesc_group
 */
const char *sec_privilege_display_name(sec_privilege privilege);

/** map a privilege name to a privilege id. Return -1 if not found
 *
 * @ingroup privilege_secdesc_group
 */
sec_privilege sec_privilege_id(const char *name);

/** return a privilege mask given a privilege id
 *
 * @ingroup privilege_secdesc_group
 */
guint64 sec_privilege_mask(sec_privilege privilege);

G_END_DECLS

#endif /* RREGADMIN_SECURITY_DESCRIPTOR_PRIVILEGE_H */
