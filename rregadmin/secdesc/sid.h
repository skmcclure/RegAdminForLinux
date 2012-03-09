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

#ifndef RREGADMIN_SECURITY_DESCRIPTOR_SID_H
#define RREGADMIN_SECURITY_DESCRIPTOR_SID_H

#include <glib.h>

#include <rregadmin/util/macros.h>
#include <rregadmin/util/types.h>

/**
 * @defgroup sid_secdesc_group sid handling for security descriptors
 *
 * @ingroup secdesc_group
 */

G_BEGIN_DECLS

#define SID_NULL ( "S-1-0-0" )
#define SID_WORLD_DOMAIN ( "S-1-1" )
#define SID_WORLD ( "S-1-1-0" )

#define SID_CREATOR_OWNER_DOMAIN ( "S-1-3" )
#define SID_CREATOR_OWNER ( "S-1-3-0" )
#define SID_CREATOR_GROUP ( "S-1-3-1" )

#define SID_NT_AUTHORITY ( "S-1-5" )
#define SID_NT_DIALUP ( "S-1-5-1" )
#define SID_NT_NETWORK ( "S-1-5-2" )
#define SID_NT_BATCH ( "S-1-5-3" )
#define SID_NT_INTERACTIVE ( "S-1-5-4" )
#define SID_NT_SERVICE ( "S-1-5-6" )
#define SID_NT_ANONYMOUS ( "S-1-5-7" )
#define SID_NT_PROXY ( "S-1-5-8" )
#define SID_NT_ENTERPRISE_DCS ( "S-1-5-9" )
#define SID_NT_SELF ( "S-1-5-10" )
#define SID_NT_AUTHENTICATED_USERS ( "S-1-5-11" )
#define SID_NT_RESTRICTED ( "S-1-5-12" )
#define SID_NT_TERMINAL_SERVER_USERS ( "S-1-5-13" )
#define SID_NT_REMOTE_INTERACTIVE ( "S-1-5-14" )
#define SID_NT_THIS_ORGANISATION ( "S-1-5-15" )
#define SID_NT_SYSTEM ( "S-1-5-18" )
#define SID_NT_LOCAL_SERVICE ( "S-1-5-19" )
#define SID_NT_NETWORK_SERVICE ( "S-1-5-20" )

#define SID_BUILTIN ( "S-1-5-32" )
#define SID_BUILTIN_ADMINISTRATORS ( "S-1-5-32-544" )
#define SID_BUILTIN_USERS ( "S-1-5-32-545" )
#define SID_BUILTIN_GUESTS ( "S-1-5-32-546" )
#define SID_BUILTIN_POWER_USERS ( "S-1-5-32-547" )
#define SID_BUILTIN_ACCOUNT_OPERATORS ( "S-1-5-32-548" )
#define SID_BUILTIN_SERVER_OPERATORS ( "S-1-5-32-549" )
#define SID_BUILTIN_PRINT_OPERATORS ( "S-1-5-32-550" )
#define SID_BUILTIN_BACKUP_OPERATORS ( "S-1-5-32-551" )
#define SID_BUILTIN_REPLICATOR ( "S-1-5-32-552" )
#define SID_BUILTIN_RAS_SERVERS ( "S-1-5-32-553" )
#define SID_BUILTIN_PREW2K ( "S-1-5-32-554" )

typedef enum
{
    SID_DEFAULT_REV = 1,
    SID_ERROR_REV = 0xff,

    SID_DEFAULT_ID = (unsigned int)-1,
    SID_ERROR_ID = (unsigned int)-2,

    SID_ERROR_AUTH_COUNT = -1,
} sid_vals;

typedef struct Sid_ Sid;

/** Sid revision type.
 *
 * @ingroup sid_secdesc_group
 */
typedef guint8 sid_rev;

/** Sid auth count type
 *
 * @note Range: 0 to 15
 *
 * @ingroup sid_secdesc_group
 */
typedef gint8 sid_auth_count;

/** Create new empty Sid
 *
 * @ingroup sid_secdesc_group
 */
Sid* sid_new(void);

/** Create new Sid specifying the id.
 *
 * @ingroup sid_secdesc_group
 */
Sid* sid_new_id(rid_type in_id);

/** Create new Sid from string.
 *
 * This will leave in_str right past the parsed section.
 *
 * @ingroup sid_secdesc_group
 */
Sid* sid_new_parse_sddl(const char **in_str);

/** Create new Sid from string.
 *
 * This will leave in_str right past the parsed section.
 *
 * @ingroup sid_secdesc_group
 */
Sid* sid_new_parse_sddl_quick(const char *in_str);

/** Create new Sid from binary data.
 *
 * @note Changes in_size to match the amount used.
 *
 * @ingroup sid_secdesc_group
 */
Sid* sid_new_parse_binary(const guint8 *in_data, gssize *in_size);

/** Copy a Sid.
 *
 * @ingroup sid_secdesc_group
 */
Sid* sid_copy(const Sid *in_sid);

/** Free a Sid.
 *
 * @ingroup sid_secdesc_group
 */
gboolean sid_free(Sid *in_sid);


/** Get sid's rev number.
 *
 * @ingroup sid_secdesc_group
 */
sid_rev sid_get_rev(const Sid *in_sid);

/** Get sid's primary id.
 *
 * @ingroup sid_secdesc_group
 */
rid_type sid_get_id(const Sid *in_sid);

/** Set sid's primary id.
 *
 * @ingroup sid_secdesc_group
 */
gboolean sid_set_id(Sid *in_sid, rid_type in_id);

/** Get the number of sid's subauths.
 *
 * @ingroup sid_secdesc_group
 */
sid_auth_count sid_get_subauth_count(const Sid *in_sid);

/** Push a new subauth onto the sid.
 *
 * @ingroup sid_secdesc_group
 */
gboolean sid_push_subauth(Sid *in_sid, rid_type in_auth);

/** Pop a new subauth onto the sid.
 *
 * @ingroup sid_secdesc_group
 */
gboolean sid_pop_subauth(Sid *in_sid);

/** Get the subauth at index.
 *
 * @ingroup sid_secdesc_group
 */
rid_type sid_get_subauth(const Sid *in_sid, gint8 in_index);

/** Get the last subauth.
 *
 * @ingroup sid_secdesc_group
 */
rid_type sid_get_last_subauth(const Sid *in_sid);

/** Compare two Sids.
 *
 * @ingroup sid_secdesc_group
 */
int sid_compare(const Sid *sid1, const Sid *sid2);

/** Compare two Sids for equality.
 *
 * @ingroup sid_secdesc_group
 */
gboolean sid_equal(const Sid *sid1, const Sid *sid2);

/** Get a binary representation of the Sid.
 *
 * @ingroup sid_secdesc_group
 */
GByteArray* sid_to_binary(const Sid *in_sid);

/** Write binary rep to byte array.
 *
 * @ingroup sid_secdesc_group
 */
gboolean sid_write_to_binary(const Sid *in_sid, GByteArray *in_arr);

/** Convert a Sid to a string.
 *
 * @ingroup sid_secdesc_group
 */
const char* sid_to_sddl(const Sid *sid);


/** Get the number of allocated Sids.
 *
 * @internal
 *
 * @ingroup sid_secdesc_group
 */
guint32 sid_allocation_count(void);


G_END_DECLS

#endif /* RREGADMIN_SECURITY_DESCRIPTOR_SID_H */
