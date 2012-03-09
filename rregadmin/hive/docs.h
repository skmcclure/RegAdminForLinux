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

#ifndef RREGADMIN_HIVE_DOCS_H
#define RREGADMIN_HIVE_DOCS_H 1

/**
 * @defgroup hive_group Hive Fileformat Handling Objects and Code
 *
 * <h3>A Brief Primer on the Hive format</h3>
 *
 * A hive consists of a header followed by a series of Bins.  Each bin's size
 * is a multiple of the block size (4096), so 4096, 8192, 12288, etc.
 *
 * Each Bin_ consists of a header followed by a series of cells.  Cells don't
 * have strict size requirements.  After each cell is another cell until the
 * whole Bin_ is filled.
 *
 * Cells come in different types: keys, value keys, key lists, value lists,
 * and values.
 *
 * Everything links to each other based on offsets from the start of the first
 * Bin_.
 *
 * <dl>
 * <dt>Hive_          <dd>[HiveHeader][Bin_][Bin_]...[Bin_]
 *
 * <dt>Bin_           <dd>[BinHeader][Cell_][Cell_]...[Cell_]
 *
 * <dt>Cell_          <dd> [KeyCell_]
 *                        | [ValueKeyCell_]
 *                        | [KeyListCell_]
 *                        | [ValueListCell_]
 *                        | [ValueCell_]
 *
 * <dt>KeyCell_       <dd>A registry key
 *
 * <dt>ValueKeyCell_  <dd>A registry value
 *
 * <dt>KeyListCell_   <dd>A list of offsets to KeyCells
 *
 * <dt>ValueListCell_ <dd>A list of offsets to ValueKeyCells
 *
 * <dt>ValueCell_     <dd>The actual data of a value.  No header, just
 *                       raw binary
 * </dl>
 *
 * <h3>An Explanation Of The Whole KeyCell_ Mess</h3>
 *
 * Every key record has a list of subkeys.  This list is refered to
 * here as a KeyListCell_.
 *
 * Sadly there are many different types of subkey lists.  A KeyListCell_ is a
 * generic reference to a list of some type or another.  Things that operate on
 * KeyListCells_ keys must do different things with the list, depending on
 * its type.
 *
 * We find type 'id' in KeyListCellHeader.  This id defines the list type:
 *
 * <dl>
 * <dt>LI
 *      <dd>(Used in Windows 3.x)
 *
 *      The LI key is just a list of offsets to KeyCell_ records.
 *
 *      I haven't delt with LI keys.  They probably don't work with this
 *      software at this time.
 *
 * <dt>LF
 *      <dd>(Used in Windows NT4.0+?)
 *
 *      Like the LI the LF key holds a list of offsets to KeyCell_ records.
 *      On top of this it also stores the first four characters of the names
 *      of the KeyCell_ records.  With LI one has to look at all of the
 *      KeyCell_ children to find the one that is desired.  With LF one
 *      only need to look at all of the records that share the first four
 *      characters.
 *
 *      I know what you are thinking.  Why on earth would they use the first
 *      four characters rather than a hash?
 *
 * <dt>LH
 *      <dd>(NT5.0 onward?)
 *
 *      This type is very similar to an LF record.  The difference is that they
 *      store a hash of the KeyCell_ name rather than the first four
 *      characters of the name.  Isn't that nice?
 *
 * <dt>RI
 *      <dd>The RI key is an ugly, ugly hack that is employed when a
 *      KeyListCell_ grows so morbidly obese that it can't be stored anymore.
 *
 *      Rather than storing a list of KeyCells_ the RI stores a list of
 *      offsets to LH records.
 * </dl>
 *
 */

#endif // RREGADMIN_HIVE_DOCS_H
