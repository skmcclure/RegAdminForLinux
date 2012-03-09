/*
 * Authors:     James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2008 Racemi Inc
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

#ifndef RREGADMIN_HIVE_INIT_H
#define RREGADMIN_HIVE_INIT_H 1

#include <rregadmin/rra_config.h>

#include <glib.h>

#include <rregadmin/util/macros.h>

/**
 * @defgroup init_hive_group Initialization.
 *
 * @ingroup hive_group
 */
G_BEGIN_DECLS

/** Initialize the hive module.
 *
 * @ingroup init_hive_group
 */
void rra_hive_init(void);

/** Initialize the hive module from env.
 *
 * @ingroup init_hive_group
 */
void rra_hive_init_from_env(void);


G_END_DECLS

#endif // RREGADMIN_HIVE_INIT_H
