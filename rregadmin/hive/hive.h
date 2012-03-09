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


#ifndef RREGADMIN_HIVE_HIVE_H
#define RREGADMIN_HIVE_HIVE_H 1

#include <rregadmin/util/macros.h>
#include <rregadmin/hive/types.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/offsets.h>
#include <rregadmin/util/offset_list.h>

/**
 * @defgroup hive_hive_group Hive Object
 *
 * This object handles reading and writing hives as well as access to the
 * hive header data, the root key cell and bins.
 *
 * @ingroup hive_group
 */
G_BEGIN_DECLS

/** Determine whether a file is a hive.
 *
 * This will not do a case insensitive open of filename.
 *
 * @ingroup hive_hive_group
 */
gboolean hive_is_hive (const char *filename);

/** Read a file as a hive and return the object.
 *
 * This will do a case insensitive open of filename.
 *
 * @ingroup hive_hive_group
 */
Hive *hive_read (const char *filename);

/** Write out a hive to a file.
 *
 * @ingroup hive_hive_group
 */
gboolean hive_write (Hive *in_hive, const char *filename);

/** Create a hive from the data provided.
 *
 * @ingroup hive_hive_group
 */
Hive *hive_create(const char *name, guint8 *buffer, int len);

/** Preload all the hive objects.
 *
 * @ingroup hive_hive_group
 */
void hive_preload(Hive *in_hive);

/** Create an basic hive structure in memory.
 *
 * @ingroup hive_hive_group
 */
Hive *hive_new (const ustring *name);

/** Free a hive.
 *
 * This is important because all Cell_ and Bin_ pointers are cached in
 * the hive structure.
 *
 * @ingroup hive_hive_group
 */
gboolean hive_free (Hive *in_hive);

/** Check the validity of a hive.
 *
 * @ingroup hive_hive_group
 */
gboolean hive_is_valid(const Hive *in_hive);

/** Get the size of a hive.
 *
 * @ingroup hive_hive_group
 */
guint32 hive_size(const Hive *hv);

/** Get the size of the data area.
 *
 * @ingroup hive_hive_group
 */
guint32 hive_get_data_size(const Hive *in_hive);

/** Get the amount of empty, unused space at end of the hive.
 *
 * @ingroup hive_hive_group
 */
guint32 hive_get_trailer_size(const Hive *in_hive);

/** Get the version of a hive.
 *
 * @ingroup hive_hive_group
 */
const HiveVersion* hive_get_version(const Hive *in_hive);

/** Get the root key of the hive.
 *
 * @ingroup hive_hive_group
 */
KeyCell *hive_get_root_key (Hive *in_hive);

/** Get the first security descriptor cell in the hive.
 *
 * @ingroup hive_hive_group
 */
SecurityDescriptorCell* hive_get_first_security_descriptor_cell(Hive *in_hive);

/** Get the name of a hive.
 *
 * \note you must free the ustring returned.
 *
 * @ingroup hive_hive_group
 */
ustring *hive_get_name (const Hive *hv) G_GNUC_MALLOC;

/** Get the state of a hive.
 *
 * @ingroup hive_hive_group
 */
int hive_get_state(const Hive *hv);

/** Get the filename of the hive.
 *
 * \note this doesn't actually have to be the filename.  If hive_create()
 *       was used it's whatever was passed as the name.
 *
 * @ingroup hive_hive_group
 */
const char *hive_get_filename(const Hive *hv);

/** Mark the hive as modified.
 *
 * @internal
 *
 * @ingroup hive_hive_group
 */
void hive_set_dirty(Hive *hv);

/** Is the hive modified?
 *
 * @ingroup hive_hive_group
 */
gboolean hive_is_dirty(Hive *hv);

/** Get the number of pages in the hive.
 *
 * A page is 4096 bytes.
 *
 * @ingroup hive_hive_group
 */
guint32 hive_get_page_count(const Hive *hv);

/** Get the number of bins in a hive.
 *
 * @ingroup hive_hive_group
 */
guint32 hive_get_bin_count(const Hive *hv);

/** Get the offset pointing to the trailer.
 *
 * @ingroup hive_hive_group
 */
offset hive_get_trailer_offset(const Hive *in_hive);

/** Get the first bin in a hive.
 *
 * @ingroup hive_hive_group
 */
Bin *hive_get_first_bin (Hive *in_hive);

/** Get the last bin in a hive.
 *
 * @ingroup hive_hive_group
 */
Bin *hive_get_last_bin (const Hive *in_hive);

/** Collect some statistics about a hive.
 *
 * @ingroup hive_hive_group
 */
void hive_collect_stats (Hive *in_hive, struct used_stats *stats);

/** Get cell at offset.
 *
 * @internal
 *
 * @ingroup hive_hive_group
 */
Cell* hive_get_cell (Hive *in_hive, offset in_ofs);

/** Get cell from integral offset.
 *
 * @internal
 *
 * @ingroup hive_hive_group
 */
Cell* hive_get_cell_from_offset(Hive *hv, guint in_ofs);

/** Find the bin containing the given offset.
 *
 * @internal
 *
 * @ingroup hive_hive_group
 */
Bin* hive_find_bin (Hive *in_hive, offset in_ofs);

/** Find a free cell within the hive of at least the given size.
 *
 * This will increase the size of the hive if necessary.
 *
 * @internal
 *
 * @ingroup hive_hive_group
 */
Cell* hive_find_free_cell (Hive *in_hive, int size);

/** Get an offset based on an integral offset.
 *
 * @internal
 *
 * @ingroup hive_hive_group
 */
offset hive_get_offset(Hive *in_hive, guint32 in_ofs);

/** Check to see if the offset is valid.
 *
 * @internal
 *
 * @ingroup hive_hive_group
 */
gboolean hive_is_valid_offset(const Hive *in_hive, guint32 in_ofs);

/** Get a list of the cell offsets in the hive.
 *
 * @ingroup hive_hive_group
 */
offset_list* hive_get_cell_offset_list(Hive *in_hive);

/** Register a Cell_ with the hive.
 *
 * @internal
 *
 * @ingroup hive_hive_group
 */
gboolean hive_register_cell(Hive *in_hive, Cell *c);

/** Unregister a Cell_ from the hive.
 *
 * @internal
 *
 * @ingroup hive_hive_group
 */
gboolean hive_unregister_cell(Hive *in_hive, Cell *c);

/** Get a previously registered Cell_ from the hive.
 *
 * @internal
 *
 * @ingroup hive_hive_group
 */
gpointer hive_retrieve_cell(Hive *hv, offset in_ofs);

/** Print hive info in xml format to stderr.
 *
 * @internal
 *
 * @ingroup hive_hive_group
 */
void hive_debug_print (const Hive *in_hive, gboolean in_verbose);

/** Print hive info in xml format to in_output.
 *
 * @internal
 *
 * @ingroup hive_hive_group
 */
gboolean hive_get_xml_output (const Hive *in_hive, ustring *in_output,
                              gboolean in_verbose);

G_END_DECLS

#endif // RREGADMIN_HIVE_HIVE_H
