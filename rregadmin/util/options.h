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

#ifndef RREGADMIN_UTIL_OPTIONS_H
#define RREGADMIN_UTIL_OPTIONS_H 1

#include <rregadmin/rra_config.h>

#include <glib.h>

#include <rregadmin/util/macros.h>

#ifdef RRA_HAVE_POPT_H
#include <popt.h>
#endif

/**
 * @defgroup options_util_group Basic API Options
 *
 * @ingroup util_group
 */
G_BEGIN_DECLS

/** Get the options group for the basic api.
 *
 * @ingroup options_util_group
 */
#ifdef RRA_HAVE_POPT_H
const struct poptOption* rra_util_options_get(void);
#else
void* rra_util_options_get(void);
#endif

/** Initialize from an environment var.
 *
 * If env_var == NULL then use env var RRA_UTIL_OPTIONS.
 *
 * @ingroup options_util_group
 */
gboolean rra_util_options_load_from_env(const char *env_var);

/** Reset options.
 *
 * @ingroup options_util_group
 */
void rra_util_options_reset(void);

/** Get whether we should turn on memory debugging.
 *
 * @ingroup options_util_group
 */
gboolean rra_util_should_debug_memory(void);


G_END_DECLS

#endif // RREGADMIN_UTIL_OPTIONS_H
