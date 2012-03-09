/*
 * Authors:     James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006,2008 Racemi Inc
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
#include <stdio.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/util/check.h>

static const char*
get_verbosity_tag(RRACheckVerbosity in_verbosity)
{
    switch (in_verbosity)
    {
    case RRA_CHECK_VERBOSITY_ERROR:
        return "E";
    case RRA_CHECK_VERBOSITY_WARNING:
        return "W";
    case RRA_CHECK_VERBOSITY_MESSAGE:
        return "M";
    case RRA_CHECK_VERBOSITY_INFO:
        return "I";
    case RRA_CHECK_VERBOSITY_DEBUG:
        return "D";
    default:
        return "?";
    }
}

void
rra_check_logv(RRACheckData *in_data, RRACheckVerbosity in_verbosity,
               const char *in_format, va_list in_args)
{
    if (in_data->verbosity >= in_verbosity)
    {
        printf ("%s: ", get_verbosity_tag(in_verbosity));
        vprintf (in_format, in_args);
        printf ("\n");
    }
}

void
rra_check_log(RRACheckData *in_data, RRACheckVerbosity in_verbosity,
              const char *in_format, ...)
{
    va_list ap;
    va_start (ap, in_format);

    rra_check_logv(in_data, in_verbosity, in_format, ap);

    va_end (ap);
}
