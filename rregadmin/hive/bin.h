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

#ifndef RREGADMIN_HIVE_BIN_H
#define RREGADMIN_HIVE_BIN_H 1

#include <rregadmin/hive/types.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/util/offsets.h>

/**
 * @defgroup bin_hive_group Bin Object
 *
 * This object contains the code and data to handle bins in hives.
 *
 * @ingroup hive_group
 */
G_BEGIN_DECLS

/** Initialize a newly allocated bin.
 *
 * @ingroup bin_hive_group
 */
gboolean bin_init(Hive *hv, offset data, guint32 data_len,
                  Bin* previous_bin);

/** Create a Bin_ object from the data starting at offset p.
 *
 * @ingroup bin_hive_group
 */
Bin *bin_get (Hive *hdesc, Bin *in_prev, offset p);

/** Delete a Bin_ object.
 *
 * \note this doesn't delete the bin in the hive.  There is no capability
 *       to do that.
 *
 * @ingroup bin_hive_group
 */
void bin_delete(Bin *p);

/** Get the first cell in the bin.
 *
 * @ingroup bin_hive_group
 */
Cell *bin_first_cell (Bin *p);

/** Get this bins offset.
 *
 * @ingroup bin_hive_group
 */
offset bin_get_offset(Bin *p);

/** Mark the bin as modified.
 *
 * @ingroup bin_hive_group
 */
gboolean bin_set_dirty (Bin *p);

/** Get the bin after this one.
 *
 * If this is the last bin return NULL.
 *
 * @ingroup bin_hive_group
 */
Bin *bin_get_next_bin (Bin *p);

/** Get the size of the bin.
 *
 * @ingroup bin_hive_group
 */
guint32 bin_size(Bin *p);

/** Get the number of pages this bin spans.
 *
 * Should be bin_size() / 4096
 *
 * @ingroup bin_hive_group
 */
guint32 bin_pages(Bin *p);

/** Get the distance to the start of the first bin.
 *
 * @ingroup bin_hive_group
 */
guint32 bin_offset_to_begin(Bin *p);

/** Check the validity of the bin.
 *
 * @ingroup bin_hive_group
 */
gboolean bin_is_valid (Bin *p);

/** Collect some statistics on the bin.
 *
 * @ingroup bin_hive_group
 */
gboolean bin_scan (Bin *hbp, struct used_stats *stats);

/** Find a free cell in the bin.
 *
 * If no free cell of at least size isn't found return NULL.
 *
 * @ingroup bin_hive_group
 */
Cell * bin_find_free_cell (Bin *p, guint32 size);

/** Check to see if the offset is within the bin.
 *
 * @ingroup bin_hive_group
 */
gboolean bin_within(Bin *p, offset check_ptr);

/** Print xml representation of the bin to stderr.
 *
 * @internal
 *
 * @ingroup bin_hive_group
 */
void bin_debug_print (Bin *p, gboolean in_verbose);

/** Print hive info in xml format to in_output.
 *
 * @internal
 *
 * @ingroup bin_hive_group
 */
gboolean bin_get_xml_output (const Bin *in_bin, ustring *in_output,
                             gboolean in_verbose);

/** Get the length of the page
 *
 * @internal
 *
 * @ingroup bin_hive_group
 */
gint32 bin_get_len_page(Bin *p);

/** Get the length of ofs_next
 *
 * @internal
 *
 * @ingroup bin_hive_group
 */
gint32 bin_get_ofs_next(Bin *p);

G_END_DECLS

#endif // RREGADMIN_HIVE_BIN_H
