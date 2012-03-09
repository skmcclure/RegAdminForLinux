/*
 * Authors:	James Lewismoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi
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

#include <rregadmin/config.h>

#include <libintl.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/registry/init.h>

#include <rregadmin/registry/log.h>
#include <rregadmin/util/init.h>
#include <rregadmin/hive/init.h>

static gboolean rra_registry_initialized = FALSE;

void
rra_registry_init(void)
{
    if (!rra_registry_initialized)
    {
        rra_info(N_("Initializing registry module"));
        rra_util_init();
        rra_hive_init();
        rra_registry_initialized = TRUE;
    }
}

static gboolean rra_registry_env_initialized = FALSE;

void
rra_registry_init_from_env(void)
{
    rra_registry_init();
    if (!rra_registry_env_initialized)
    {
        rra_info(N_("Initializing registry module from env"));
        rra_util_init_from_env();
        rra_hive_init_from_env();
        rra_registry_env_initialized = TRUE;
    }
}
