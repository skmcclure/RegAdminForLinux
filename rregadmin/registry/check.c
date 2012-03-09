/*
 * Authors:     James LewisMoss <jlm@racemi.com>
 *
 * Copyright 2008 Racemi Inc
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

#include <glib/gi18n-lib.h>

#include <rregadmin/registry/check.h>

#include <rregadmin/registry/log.h>
#include <rregadmin/registry/registry.h>
#include <rregadmin/registry/registry_key.h>


gboolean
rcheck_required_hives(const Registry *in_reg,
                      RRACheckData *in_data)
{
    if (in_reg == NULL || in_data == NULL)
    {
        return FALSE;
    }

    return FALSE;
}

gboolean
rcheck_required_links(const Registry *in_reg,
                      RRACheckData *in_data)
{
    if (in_reg == NULL || in_data == NULL)
    {
        return FALSE;
    }

    return FALSE;
}

gboolean
registry_check(const Registry *in_reg,
               RRACheckData *in_data,
               gboolean in_include_per_hive_checks)
{
    if (in_reg == NULL || in_data == NULL)
    {
        return FALSE;
    }

    if (in_include_per_hive_checks)
    {
    }

    return FALSE;
}
