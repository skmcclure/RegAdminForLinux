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

#ifndef RREGADMIN_CHECK_REGISTRY_H
#define RREGADMIN_CHECK_REGISTRY_H 1

#include <glib.h>

#include <rregadmin/registry/types.h>
#include <rregadmin/util/check.h>

G_BEGIN_DECLS

/**
 * @defgroup registry_check_group Check Registry
 *
 * Functions to check a registry for errors.
 *
 * @ingroup registry_group
 */

/** Check a registry for errors.
 *
 * @ingroup registry_check_group
 */
gboolean registry_check(const Registry *in_reg,
                        RRACheckData *in_data,
                        gboolean in_include_per_hive_checks);

/** Check that a registry has all the required hives.
 *
 * @ingroup registry_check_group
 */
gboolean rcheck_required_hives(const Registry *in_reg,
                               RRACheckData *in_data);

/** Check that a registry has all the required links.
 *
 * @ingroup registry_check_group
 */
gboolean rcheck_required_links(const Registry *in_reg,
                               RRACheckData *in_data);



G_END_DECLS

#endif // RREGADMIN_CHECK_REGISTRY_H
