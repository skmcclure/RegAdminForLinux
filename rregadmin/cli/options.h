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

#ifndef RREGADMIN_CLI_OPTIONS_H
#define RREGADMIN_CLI_OPTIONS_H 1

#include <rregadmin/rra_config.h>

#include <glib.h>

#include <rregadmin/util/macros.h>

#ifdef RRA_HAVE_POPT_H
#include <popt.h>
#endif

/**
 * @defgroup options_cli_group Basic API Options
 *
 * @ingroup cli_group
 */
G_BEGIN_DECLS

/** Get the options group for the basic api.
 *
 * @ingroup options_cli_group
 */
#ifdef RRA_HAVE_POPT_H
const struct poptOption* rra_cli_options_get(void);
#else
void* rra_cli_options_get(void);
#endif

/** Initialize from an environment var.
 *
 * If env_var == NULL then use env var RRA_CLI_OPTIONS.
 *
 * @ingroup options_cli_group
 */
gboolean rra_cli_options_load_from_env(const char *env_var);

/** Reset options.
 *
 * @ingroup options_cli_group
 */
void rra_cli_options_reset(void);

/** Should make hive backups.
 *
 * @ingroup options_cli_group
 */
gboolean rra_cli_options_should_make_backup(void);

/** Should check hive integrity at read time.
 *
 * @ingroup options_cli_group
 */
gboolean rra_cli_options_should_check_hive_read(void);

/** Should check hive integrity at write time.
 *
 * @ingroup options_cli_group
 */
gboolean rra_cli_options_should_check_hive_write(void);


G_END_DECLS

#endif // RREGADMIN_CLI_OPTIONS_H
