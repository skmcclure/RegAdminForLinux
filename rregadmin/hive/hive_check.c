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

#include <rregadmin/config.h>

#include <stdarg.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/hive/hive_check.h>
#include <rregadmin/hive/hcheck_decl.h>

#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/hive/key_list_cell.h>
#include <rregadmin/hive/value_list_cell.h>
#include <rregadmin/hive/value_cell.h>

struct PassInfo
{
    const char *name;
    HC_Pass fun;
};

static const struct PassInfo passes[] = {
    { "Hive", hcheck_hive_pass },
    { "Bin", hcheck_bin_pass },
    { "Cell", hcheck_cell_pass },
    { "KeyCell", hcheck_key_cell_pass },
    { "ValueKeyCell", hcheck_value_key_cell_pass },
    { "KeyListCell", hcheck_key_list_cell_pass },
    { "SecurityDescriptorCell", hcheck_security_descriptor_cell_pass },
    { "GC", hcheck_gc_pass },

    { NULL, NULL }
};

gboolean
hive_check(Hive *in_hive, RRACheckData *in_data)
{
    gboolean ret_val = TRUE;
    guint i = 0;

    rra_check_message(in_data, _("Start   : Hive %s"),
                      hive_get_filename(in_hive));

    while (passes[i].name != NULL)
    {
        rra_check_message(in_data, _("Start   : Pass: %s"),
                          passes[i].name);
        gboolean pass_ret = (*passes[i].fun)(in_hive, in_data);
        rra_check_message(in_data, _("%s : Pass: %s"),
                          (pass_ret ? _("Success") : _("FAILURE")),
                          passes[i].name);
        if (!pass_ret)
        {
            ret_val = FALSE;
        }
        i++;
    }

    rra_check_message(in_data, _("%s: Hive %s\n\n"),
                      (ret_val ? _("Success") : _("FAILURE")),
                      hive_get_filename(in_hive));

    return ret_val;
}
