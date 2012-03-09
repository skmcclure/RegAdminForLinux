/*
  Copyright 2007,2008 Racemi Inc
  Copyright Andrew Tridgell 2004

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

#ifndef RREGADMIN_SECURITY_DESCRIPTOR_SECURITY_DESCRIPTOR_H
#define RREGADMIN_SECURITY_DESCRIPTOR_SECURITY_DESCRIPTOR_H

#include <rregadmin/util/macros.h>

#include <rregadmin/secdesc/aclist.h>
#include <rregadmin/secdesc/sid.h>
#include <rregadmin/secdesc/secdesc_type.h>

/**
 * @defgroup secdesc_secdesc_group Security Descriptor object.
 *
 * @ingroup secdesc_group
 */

G_BEGIN_DECLS

typedef enum
{
    SECURITY_DESCRIPTOR_REVISION_1 = 1
} secdesc_rev_code;

/** SecurityDescriptor revision.
 *
 * @ingroup secdesc_secdesc_group
 */
typedef guint16 secdesc_rev;

/** SecurityDescriptor offset.
 *
 * @ingroup secdesc_secdesc_group
 */
typedef guint32 secdesc_offset;

typedef enum
{
    SECDESC_DEFAULT_REV = SECURITY_DESCRIPTOR_REVISION_1,
    SECDESC_ERROR_REV = (secdesc_rev)0xffff,

} secdesc_vals;


typedef struct SecurityDescriptor_ SecurityDescriptor;

/** Create new empty SecurityDescriptor
 *
 * @ingroup secdesc_secdesc_group
 */
SecurityDescriptor* secdesc_new(void);

/** Create new SecurityDescriptor specifying the id.
 *
 * @ingroup secdesc_secdesc_group
 */
SecurityDescriptor* secdesc_new_full(Sid *in_owner, Sid *in_group);

/** Create new SecurityDescriptor from string.
 *
 * This will leave in_str right past the parsed section.
 *
 * @ingroup secdesc_secdesc_group
 */
SecurityDescriptor* secdesc_new_parse_sddl(const char **in_str);

/** Create new SecurityDescriptor from string.
 *
 * This will leave in_str right past the parsed section.
 *
 * @ingroup secdesc_secdesc_group
 */
SecurityDescriptor* secdesc_new_parse_sddl_quick(const char *in_str);

/** Create new SecurityDescriptor from binary data.
 *
 * @note Changes in_size to match the amount used.
 *
 * @ingroup secdesc_secdesc_group
 */
SecurityDescriptor* secdesc_new_parse_binary(const guint8 *in_data,
                                             gsize *in_size);

/** Copy a SecurityDescriptor.
 *
 * @ingroup secdesc_secdesc_group
 */
SecurityDescriptor* secdesc_copy(const SecurityDescriptor *in_sd);

/** Free a SecurityDescriptor.
 *
 * @ingroup secdesc_secdesc_group
 */
gboolean secdesc_free(SecurityDescriptor *in_sd);

/** Get SecurityDescriptor's rev number.
 *
 * @ingroup secdesc_secdesc_group
 */
secdesc_rev secdesc_get_rev(const SecurityDescriptor *in_sd);

/** Set SecurityDescriptor's rev number.
 *
 * @ingroup secdesc_secdesc_group
 */
gboolean secdesc_set_rev(SecurityDescriptor *in_sd, secdesc_rev in_rev);

/** Get the SecurityDescriptor's type mask.
 *
 * @ingroup secdesc_secdesc_group
 */
secdesc_type secdesc_get_type(const SecurityDescriptor *in_sd);

/** Check to see if a particular code is set.
 *
 * @ingroup secdesc_secdesc_group
 */
gboolean secdesc_has_type_code(const SecurityDescriptor *in_sd,
                               secdesc_type_code in_code);

/** Set the SecurityDescriptor's type mask.
 *
 * @ingroup secdesc_secdesc_group
 */
gboolean secdesc_set_type(SecurityDescriptor *in_sd, secdesc_type in_type);

/** Set a type code.
 *
 * @ingroup secdesc_secdesc_group
 */
gboolean secdesc_set_type_code(SecurityDescriptor *in_sd,
                               secdesc_type_code in_code);

/** Unset a type code.
 *
 * @ingroup secdesc_secdesc_group
 */
gboolean secdesc_unset_type_code(SecurityDescriptor *in_sd,
                                 secdesc_type_code in_code);

/** Get the SecurityDescriptor's owner.
 *
 * @ingroup secdesc_secdesc_group
 */
const Sid* secdesc_get_owner(const SecurityDescriptor *in_sd);

/** Set the SecurityDescriptor's owner.
 *
 * @ingroup secdesc_secdesc_group
 */
gboolean secdesc_set_owner(SecurityDescriptor *in_sd, Sid *in_owner);

/** Get the SecurityDescriptor's group.
 *
 * @ingroup secdesc_secdesc_group
 */
const Sid* secdesc_get_group(const SecurityDescriptor *in_sd);

/** Set the SecurityDescriptor's group.
 *
 * @ingroup secdesc_secdesc_group
 */
gboolean secdesc_set_group(SecurityDescriptor *in_sd, Sid *in_group);

/** Get the SecurityDescriptor's sacl.
 *
 * @ingroup secdesc_secdesc_group
 */
ACList* secdesc_get_sacl(SecurityDescriptor *in_sd);

/** Get the SecurityDescriptor's dacl.
 *
 * @ingroup secdesc_secdesc_group
 */
ACList* secdesc_get_dacl(SecurityDescriptor *in_sd);

/** Compare two SecurityDescriptors.
 *
 * @ingroup secdesc_secdesc_group
 */
int secdesc_compare(const SecurityDescriptor *in_sd1,
                    const SecurityDescriptor *in_sd2);

/** Compare two SecurityDescriptors for equality.
 *
 * @ingroup secdesc_secdesc_group
 */
gboolean secdesc_equal(const SecurityDescriptor *in_sd1,
                       const SecurityDescriptor *in_sd2);

/** Get a binary representation of the SecurityDescriptor.
 *
 * @ingroup secdesc_secdesc_group
 */
GByteArray* secdesc_to_binary(const SecurityDescriptor *in_sd,
                              int in_block_size);

/** Write binary rep to byte array.
 *
 * @ingroup secdesc_secdesc_group
 */
gboolean secdesc_write_to_binary(const SecurityDescriptor *in_sd,
                                 GByteArray *in_arr, int in_block_size);

/** Convert a SecurityDescriptor to a string.
 *
 * @ingroup secdesc_secdesc_group
 */
const char* secdesc_to_sddl(const SecurityDescriptor *in_sd);


/** Get the number of allocated SecurityDescriptors.
 *
 * @internal
 *
 * @ingroup secdesc_secdesc_group
 */
guint32 secdesc_allocation_count(void);


G_END_DECLS

#endif /* RREGADMIN_SECURITY_DESCRIPTOR_SECURITY_DESCRIPTOR_H */
