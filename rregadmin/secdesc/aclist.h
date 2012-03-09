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



#ifndef RREGADMIN_SECURITY_DESCRIPTOR_ACLIST_H
#define RREGADMIN_SECURITY_DESCRIPTOR_ACLIST_H

#include <glib.h>

#include <rregadmin/util/macros.h>
#include <rregadmin/secdesc/acentry.h>

/**
 * @defgroup aclist_secdesc_group Access Control List object.
 *
 * @ingroup secdesc_group
 */

G_BEGIN_DECLS

typedef enum
{
    ACLIST_REVISION_NT4 = 2,
    ACLIST_REVISION_ADS = 4
} security_acl_revision;

typedef enum
{
    ACLIST_DEFAULT_REV = ACLIST_REVISION_NT4,
    ACLIST_ERROR_REV = 0xffff,

    ACLIST_ERROR_ACENTRY_COUNT = -1,
} acentry_vals;

/** ACList revision type.
 *
 * @ingroup aclist_secdesc_group
 */
typedef guint16 aclist_rev;

/** ACList acentry size
 *
 * @ingroup aclist_secdesc_group
 */
typedef guint32 aclist_acentry_count;

typedef struct ACList_ ACList;

/** Create new empty ACList
 *
 * @ingroup aclist_secdesc_group
 */
ACList* aclist_new(void);

/** Create new ACList from string.
 *
 * @ingroup aclist_secdesc_group
 */
ACList* aclist_new_parse_sddl(const char **in_str);

/** Create new ACList from string.
 *
 * @ingroup aclist_secdesc_group
 */
ACList* aclist_new_parse_sddl_quick(const char *in_str);

/** Create new ACList from binary data.
 *
 * @note Changes in_size to match the amount used.
 *
 * @ingroup aclist_secdesc_group
 */
ACList* aclist_new_parse_binary(const guint8 *in_data, gssize *in_size);

/** Copy a ACList.
 *
 * @ingroup aclist_secdesc_group
 */
ACList* aclist_copy(const ACList *in_aclist);

/** Free a ACList.
 *
 * @ingroup aclist_secdesc_group
 */
gboolean aclist_free(ACList *in_aclist);


/** Get aclist's rev number.
 *
 * @ingroup aclist_secdesc_group
 */
aclist_rev aclist_get_rev(const ACList *in_aclist);

/** Set the aclist's rev number.
 *
 * @ingroup aclist_secdesc_group
 */
gboolean aclist_set_rev(ACList *in_aclist, aclist_rev in_rev);

/** Get the number of aclist's acentrys.
 *
 * @ingroup aclist_secdesc_group
 */
aclist_acentry_count aclist_get_acentry_count(const ACList *in_aclist);

/** Push a new acentry onto the aclist.
 *
 * @ingroup aclist_secdesc_group
 */
gboolean aclist_push_acentry(ACList *in_aclist, ACEntry *in_entry);

/** Pop a new acentry onto the aclist.
 *
 * @ingroup aclist_secdesc_group
 */
gboolean aclist_pop_acentry(ACList *in_aclist);

/** Get the acentry at index.
 *
 * @ingroup aclist_secdesc_group
 */
const ACEntry* aclist_get_acentry(const ACList *in_aclist, gint8 in_index);

/** Compare two ACLists.
 *
 * @ingroup aclist_secdesc_group
 */
int aclist_compare(const ACList *aclist1, const ACList *aclist2);

/** Compare two ACLists for equality.
 *
 * @ingroup aclist_secdesc_group
 */
gboolean aclist_equal(const ACList *aclist1, const ACList *aclist2);

/** Get a binary representation of the ACList.
 *
 * @ingroup aclist_secdesc_group
 */
GByteArray* aclist_to_binary(const ACList *in_aclist, gssize in_block_size);

/** Write binary rep to byte array.
 *
 * @ingroup aclist_secdesc_group
 */
gboolean aclist_write_to_binary(const ACList *in_aclist, GByteArray *in_arr,
                                gssize in_block_size);

/** Convert a ACList to a string.
 *
 * @ingroup aclist_secdesc_group
 */
const char* aclist_to_sddl(const ACList *in_aclist);

/** Get the number of allocated ACEntrys
 *
 * @internal
 *
 * @ingroup acentry_secdesc_group
 */
guint32 aclist_allocation_count(void);


G_END_DECLS

#endif /* RREGADMIN_SECURITY_DESCRIPTOR_ACLIST_H */
