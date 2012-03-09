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

#include <rregadmin/util/init.h>
#include <rregadmin/util/log.h>
#include <rregadmin/util/options.h>

static gboolean rra_util_initialized = FALSE;

void
rra_util_init(void)
{
    if (!rra_util_initialized)
    {
        rra_info(N_("Initializing util module"));
        bindtextdomain(PACKAGE, LOCALEDIR);
        rra_log_initialize();
        rra_util_initialized = TRUE;
    }
}

static gboolean rra_util_env_initialized = FALSE;

void
rra_util_init_from_env(void)
{
    rra_util_init();
    if (!rra_util_env_initialized)
    {
        rra_info(N_("Initializing util module from env"));
        rra_util_options_load_from_env(NULL);
        rra_util_env_initialized = TRUE;
        if (rra_util_should_debug_memory())
        {
#ifdef HAVE_G_SLICE_ALLOC
            g_slice_set_config(G_SLICE_CONFIG_ALWAYS_MALLOC, 1);
#endif
        }
    }
}
