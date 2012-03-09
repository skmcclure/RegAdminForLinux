/*
  Copyright 2007 Racemi Inc
  Copyright 2004 Andrew Tridgell

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

#ifndef RREGADMIN_SECURITY_DESCRIPTOR_ACENTRY_H
#define RREGADMIN_SECURITY_DESCRIPTOR_ACENTRY_H

#include <rregadmin/util/macros.h>

#include <rregadmin/secdesc/sid.h>

/**
 * @defgroup acentry_secdesc_group Access Control Entry Object.
 *
 * @ingroup secdesc_group
 */

G_BEGIN_DECLS

/** ACEntry type.
 *
 * @ingroup acentry_secdesc_group
 */
typedef guint8 acentry_type;

/** acentry_type codes.
 *
 * @ingroup acentry_secdesc_group
 */
typedef enum
{
    SEC_ACE_TYPE_ACCESS_ALLOWED = 0,
    SEC_ACE_TYPE_ACCESS_DENIED = 1,
    SEC_ACE_TYPE_SYSTEM_AUDIT = 2,
    SEC_ACE_TYPE_SYSTEM_ALARM = 3,
    SEC_ACE_TYPE_ALLOWED_COMPOUND = 4,
    SEC_ACE_TYPE_ACCESS_ALLOWED_OBJECT = 5,
    SEC_ACE_TYPE_ACCESS_DENIED_OBJECT = 6,
    SEC_ACE_TYPE_SYSTEM_AUDIT_OBJECT = 7,
    SEC_ACE_TYPE_SYSTEM_ALARM_OBJECT = 8,

    SEC_ACE_TYPE_INVALID = (acentry_type)-1,
} acentry_type_code;

/** ACEntry flags type.
 *
 * @ingroup acentry_secdesc_group
 */
typedef guint8 acentry_flags;

/** acentry_flags codes.
 *
 * @ingroup acentry_secdesc_group
 */
typedef enum
{
    SEC_ACE_FLAG_OBJECT_INHERIT = 0x01,
    SEC_ACE_FLAG_CONTAINER_INHERIT = 0x02,
    SEC_ACE_FLAG_NO_PROPAGATE_INHERIT = 0x04,
    SEC_ACE_FLAG_INHERIT_ONLY = 0x08,
    SEC_ACE_FLAG_VALID_INHERIT = 0x0f,

    SEC_ACE_FLAG_INHERITED_ACE = 0x10,
    SEC_ACE_FLAG_SUCCESSFUL_ACCESS = 0x40,
    SEC_ACE_FLAG_FAILED_ACCESS = 0x80,

    SEC_ACE_FLAG_NULL = 0x00,
} acentry_flags_code;

/** acentry_access_mask codes.
 *
 * @ingroup acentry_secdesc_group
 */
typedef enum
{
    SEC_MASK_GENERIC =  0xF0000000,
    SEC_MASK_FLAGS =    0x0F000000,
    SEC_MASK_STANDARD = 0x00FF0000,
    SEC_MASK_SPECIFIC = 0x0000FFFF,

    SEC_GENERIC_ALL =     0x10000000,
    SEC_GENERIC_EXECUTE = 0x20000000,
    SEC_GENERIC_WRITE =   0x40000000,
    SEC_GENERIC_READ =    0x80000000,

    SEC_FLAG_SYSTEM_SECURITY = 0x01000000,
    SEC_FLAG_MAXIMUM_ALLOWED = 0x02000000,

    SEC_STD_DELETE =       0x00010000,
    SEC_STD_READ_CONTROL = 0x00020000,
    SEC_STD_WRITE_DAC =    0x00040000,
    SEC_STD_WRITE_OWNER =  0x00080000,
    SEC_STD_SYNCHRONIZE =  0x00100000,
    SEC_STD_REQUIRED =     0x000F0000,
    SEC_STD_ALL =          0x001F0000,

    SEC_FILE_READ_DATA =       0x00000001,
    SEC_FILE_WRITE_DATA =      0x00000002,
    SEC_FILE_APPEND_DATA =     0x00000004,
    SEC_FILE_READ_EA =         0x00000008,
    SEC_FILE_WRITE_EA =        0x00000010,
    SEC_FILE_EXECUTE =         0x00000020,
    SEC_FILE_READ_ATTRIBUTE =  0x00000080,
    SEC_FILE_WRITE_ATTRIBUTE = 0x00000100,
    SEC_FILE_ALL =             0x000001ff,

    SEC_DIR_LIST =             0x00000001,
    SEC_DIR_ADD_FILE =         0x00000002,
    SEC_DIR_ADD_SUBDIR =       0x00000004,
    SEC_DIR_READ_EA =          0x00000008,
    SEC_DIR_WRITE_EA =         0x00000010,
    SEC_DIR_TRAVERSE =         0x00000020,
    SEC_DIR_DELETE_CHILD =     0x00000040,
    SEC_DIR_READ_ATTRIBUTE =   0x00000080,
    SEC_DIR_WRITE_ATTRIBUTE =  0x00000100,

    SEC_REG_QUERY_VALUE =      0x00000001,
    SEC_REG_SET_VALUE =        0x00000002,
    SEC_REG_CREATE_SUBKEY =    0x00000004,
    SEC_REG_ENUM_SUBKEYS =     0x00000008,
    SEC_REG_NOTIFY =           0x00000010,
    SEC_REG_CREATE_LINK =      0x00000020,

    SEC_ADS_CREATE_CHILD =     0x00000001,
    SEC_ADS_DELETE_CHILD =     0x00000002,
    SEC_ADS_LIST =             0x00000004,
    SEC_ADS_SELF_WRITE =       0x00000008,
    SEC_ADS_READ_PROP =        0x00000010,
    SEC_ADS_WRITE_PROP =       0x00000020,
    SEC_ADS_DELETE_TREE =      0x00000040,
    SEC_ADS_LIST_OBJECT =      0x00000080,
    SEC_ADS_CONTROL_ACCESS =   0x00000100,

    SEC_RIGHTS_FILE_READ = (SEC_STD_READ_CONTROL
                            | SEC_STD_SYNCHRONIZE
                            | SEC_FILE_READ_DATA
                            | SEC_FILE_READ_ATTRIBUTE
                            | SEC_FILE_READ_EA),
    SEC_RIGHTS_FILE_WRITE = (SEC_STD_READ_CONTROL
                             | SEC_STD_SYNCHRONIZE
                             | SEC_FILE_WRITE_DATA
                             | SEC_FILE_WRITE_ATTRIBUTE
                             | SEC_FILE_WRITE_EA
                             | SEC_FILE_APPEND_DATA),
    SEC_RIGHTS_FILE_EXECUTE = (SEC_STD_SYNCHRONIZE
                               | SEC_STD_READ_CONTROL
                               | SEC_FILE_READ_ATTRIBUTE
                               | SEC_FILE_EXECUTE),
    SEC_RIGHTS_FILE_ALL = SEC_STD_ALL | SEC_FILE_ALL,

    SEC_RIGHTS_DIR_READ = SEC_RIGHTS_FILE_READ,
    SEC_RIGHTS_DIR_WRITE = SEC_RIGHTS_FILE_WRITE,
    SEC_RIGHTS_DIR_EXECUTE = SEC_RIGHTS_FILE_EXECUTE,
    SEC_RIGHTS_DIR_ALL = SEC_RIGHTS_FILE_ALL,

    SEC_RIGHTS_REG_READ = SEC_RIGHTS_FILE_READ,
    SEC_RIGHTS_REG_WRITE = SEC_RIGHTS_FILE_WRITE,
    SEC_RIGHTS_REG_EXECUTE = SEC_RIGHTS_FILE_EXECUTE,
    SEC_RIGHTS_REG_ALL = SEC_RIGHTS_FILE_ALL,

    SEC_RIGHTS_ADS_ALL = (SEC_ADS_CREATE_CHILD
                          | SEC_ADS_DELETE_CHILD
                          | SEC_ADS_LIST
                          | SEC_ADS_SELF_WRITE
                          | SEC_ADS_READ_PROP
                          | SEC_ADS_WRITE_PROP
                          | SEC_ADS_DELETE_TREE
                          | SEC_ADS_LIST_OBJECT
                          | SEC_ADS_CONTROL_ACCESS),


} acentry_access_masks_code;
/** ACEntry access mask type.
 *
 * @ingroup acentry_secdesc_group
 */
typedef guint32 acentry_access_mask;

/* bitmap security_ace_object_flags */
#define SEC_ACE_OBJECT_TYPE_PRESENT ( 0x00000001 )
#define SEC_ACE_INHERITED_OBJECT_TYPE_PRESENT ( 0x00000002 )

typedef struct ACEntry_ ACEntry;

/** Create new empty ACEntry
 *
 * @ingroup acentry_secdesc_group
 */
ACEntry* acentry_new(void);

/** Create new ACEntry from string.
 *
 * @ingroup acentry_secdesc_group
 */
ACEntry* acentry_new_parse_sddl(const char **in_str);

/** Create new ACEntry from string.
 *
 * @ingroup acentry_secdesc_group
 */
ACEntry* acentry_new_parse_sddl_quick(const char *in_str);

/** Create new ACEntry from binary data.
 *
 * @note Changes in_size to match the amount used.
 *
 * @ingroup acentry_secdesc_group
 */
ACEntry* acentry_new_parse_binary(const guint8 *in_data, gssize *in_size);

/** Copy an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
ACEntry* acentry_copy(const ACEntry *in_acentry);

/** Free an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
gboolean acentry_free(ACEntry *in_acentry);


/** Get type code for an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
acentry_type acentry_get_type(const ACEntry *in_acentry);

/** Set type code for an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
gboolean acentry_set_type(ACEntry *in_acentry, acentry_type in_type);

/** Get flags for an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
acentry_flags acentry_get_flags(const ACEntry *in_acentry);

/** Check to see if one flag is set in an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
gboolean acentry_has_flag(const ACEntry *in_acentry, acentry_flags in_flags);

/** Set flags for an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
gboolean acentry_set_flags(ACEntry *in_acentry, acentry_flags in_flags);

/** Set one flag in an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
gboolean acentry_add_flag(ACEntry *in_acentry, acentry_flags in_flags);

/** Unset one flag in an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
gboolean acentry_del_flag(ACEntry *in_acentry, acentry_flags in_flags);

/** Get access_mask for an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
acentry_access_mask acentry_get_access_mask(const ACEntry *in_acentry);

/** Check to see if one access_mask is set in an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
gboolean acentry_has_access_mask(const ACEntry *in_acentry,
                                 acentry_access_mask in_mask);

/** Set access mask for an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
gboolean acentry_set_access_mask(ACEntry *in_acentry,
                                 acentry_access_mask in_mask);

/** Set one access_mask in an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
gboolean acentry_add_access_mask(ACEntry *in_acentry,
                                 acentry_access_mask in_mask);

/** Unset one access_mask in an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
gboolean acentry_del_access_mask(ACEntry *in_acentry,
                                 acentry_access_mask in_mask);

/** Get Sid for an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
const Sid* acentry_get_trustee(const ACEntry *in_acentry);

/** Set the Sid for an ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
gboolean acentry_set_trustee(ACEntry *in_acentry, Sid *in_sid);


/** Compare two ACEntries.
 *
 * @ingroup acentry_secdesc_group
 */
int acentry_compare(const ACEntry *ace1, const ACEntry *ace2);

/** Compare two ACEntries for equality.
 *
 * @ingroup acentry_secdesc_group
 */
gboolean acentry_equal(const ACEntry *ace1, const ACEntry *ace2);

/** Get a binary representation of the ACEntry.
 *
 * @ingroup acentry_secdesc_group
 */
GByteArray* acentry_to_binary(const ACEntry *in_acentry);

/** Write binary rep to byte array.
 *
 * @ingroup acentry_secdesc_group
 */
gboolean acentry_write_to_binary(const ACEntry *in_acentry,
                                 GByteArray *in_arr);

/** Convert a ACEntry to a string.
 *
 * @ingroup acentry_secdesc_group
 */
const char* acentry_to_sddl(const ACEntry *in_acentry);


/** Get the number of allocated ACEntrys
 *
 * @internal
 *
 * @ingroup acentry_secdesc_group
 */
guint32 acentry_allocation_count(void);


G_END_DECLS

#endif /* RREGADMIN_SECURITY_DESCRIPTOR_ACENTRY_H */
