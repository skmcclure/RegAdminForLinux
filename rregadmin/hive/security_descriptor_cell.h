/*
 * Authors:	Sean Loaring
 *		Petter Nordahl-Hagen
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

#ifndef RREGADMIN_HIVE_SECURITY_DESCRIPTOR_CELL_H
#define RREGADMIN_HIVE_SECURITY_DESCRIPTOR_CELL_H 1

#include <rregadmin/util/macros.h>
#include <rregadmin/util/offsets.h>
#include <rregadmin/hive/types.h>

#include <rregadmin/secdesc/security_descriptor.h>

/**
 * @defgroup sdc_hive_group SecurityDescriptorCell Object
 *
 * @ingroup hive_group
 */
G_BEGIN_DECLS

/** Allocate a security descriptor cell in the hive.
 *
 * If a SecurityDescriptorCell exists for the SecurityDescriptor already
 * just return that one after incrementing the use count.
 *
 * @ingroup sdc_hive_group
 */
SecurityDescriptorCell * security_descriptor_cell_alloc (
    Hive *in_hive, offset p, const SecurityDescriptor *in_secdesc);

/** Deallocate a security descriptor cell in the hive.
 *
 * @ingroup sdc_hive_group
 */
gboolean security_descriptor_cell_unalloc(SecurityDescriptorCell *in_sdc);

/** Convert a Cell_ to a SecurityDescriptorCell_
 *
 * @ingroup sdc_hive_group
 */
SecurityDescriptorCell* security_descriptor_cell_from_cell (Cell *p);


/** Convert a SecurityDescriptorCell_ to a Cell_.
 *
 * @ingroup sdc_hive_group
 */
Cell* security_descriptor_cell_to_cell(SecurityDescriptorCell *in_sdc);

/** Get the use count.
 *
 * @ingroup sdc_hive_group
 */
int security_descriptor_cell_get_use_count(SecurityDescriptorCell *in_sdc);

/** Increment the use count.
 *
 * @ingroup sdc_hive_group
 */
gboolean security_descriptor_cell_increment_use_count(
    SecurityDescriptorCell *in_sdc);

/** Decrement the use count.
 *
 * @ingroup sdc_hive_group
 */
gboolean security_descriptor_cell_decrement_use_count(
    SecurityDescriptorCell *in_sdc);

/** Get the previous SecurityDescriptorCell in the hive.
 *
 * @ingroup sdc_hive_group
 */
SecurityDescriptorCell* security_descriptor_cell_get_prev(
    SecurityDescriptorCell *in_sdc);

/** Get the next SecurityDescriptorCell in the hive.
 *
 * @ingroup sdc_hive_group
 */
SecurityDescriptorCell* security_descriptor_cell_get_next(
    SecurityDescriptorCell *in_sdc);

/** Get the SecurityDescriptor for this cell.
 *
 * @note must free returned value.
 */
const SecurityDescriptor* security_descriptor_cell_get_secdesc(
    SecurityDescriptorCell *in_sdc);

/** Print an xml representation to stderr.
 *
 * @internal
 *
 * @ingroup sdc_hive_group
 */
void security_descriptor_cell_debug_print (
    SecurityDescriptorCell *in_sdc);

gboolean security_descriptor_cell_get_xml_output (
    SecurityDescriptorCell *in_sdc, ustring *in_output);

G_END_DECLS

#endif // RREGADMIN_HIVE_SECURITY_DESCRIPTOR_CELL_H
