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

#ifndef RREGADMIN_HIVE_HIVE_CHECK_H
#define RREGADMIN_HIVE_HIVE_CHECK_H 1

#include <rregadmin/util/macros.h>
#include <rregadmin/util/check.h>
#include <rregadmin/hive/types.h>

G_BEGIN_DECLS

/** Do a thorough scan of a hive checking for all known issues.
 *
 * @ingroup hive_group
 */
gboolean hive_check(Hive *in_hive, RRACheckData *in_data);

G_END_DECLS

#endif // RREGADMIN_HIVE_HIVE_CHECK_H
