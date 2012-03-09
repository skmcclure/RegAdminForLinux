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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include <rregadmin/util/log.h>

const int rra_comp_loglevel = RRA_COMPILED_LOG_LEVEL;

static const int default_log_level = G_LOG_LEVEL_WARNING;
static int current_log_level = G_LOG_LEVEL_WARNING;
static guint log_handler = 0;

static const char*
get_log_level_string(GLogLevelFlags flags)
{
    switch(flags)
    {
    case RRA_LOG_LEVEL_TRACE:
        return "trace";
    case G_LOG_LEVEL_DEBUG:
        return "debug";
    case G_LOG_LEVEL_INFO:
        return "Info";
    case G_LOG_LEVEL_MESSAGE:
        return "Message";
    case G_LOG_LEVEL_WARNING:
        return "WARNING";
    default:
        return "UnknownLevel";
    }
}

gboolean
rra_log_is_supported_level (int in_level)
{
    return in_level <= rra_comp_loglevel;
}

int
rra_log_string_to_level(const char *in_str)
{
    if (strcasecmp(in_str, "warning") == 0)
    {
        return G_LOG_LEVEL_WARNING;
    }
    else if (strcasecmp(in_str, "message") == 0)
    {
        return G_LOG_LEVEL_MESSAGE;
    }
    else if (strcasecmp(in_str, "info") == 0)
    {
        return G_LOG_LEVEL_INFO;
    }
    else if (strcasecmp(in_str, "debug") == 0)
    {
        return G_LOG_LEVEL_DEBUG;
    }
    else if (strcasecmp(in_str, "trace") == 0)
    {
        return RRA_LOG_LEVEL_TRACE;
    }
    else
    {
        return 0;
    }
}

static void
rra_glog_handler(const gchar *log_domain,
                 GLogLevelFlags log_level,
                 const gchar *message,
                 gpointer DECLARE_UNUSED(user_data))
{
    fprintf (stderr, "%s: %s/%s\n", get_log_level_string(log_level),
             log_domain, message);
}

void
rra_log_initialize(void)
{
    log_handler = g_log_set_handler(G_LOG_DOMAIN,
                                    RRA_LOG_LEVEL_TRACE
                                    | G_LOG_LEVEL_DEBUG
                                    | G_LOG_LEVEL_INFO
                                    | G_LOG_LEVEL_MESSAGE
                                    | G_LOG_LEVEL_WARNING,
                                    rra_glog_handler,
                                    NULL);
}

gboolean
rra_log_set_level(int in_level)
{
    current_log_level = in_level;
    return TRUE;
}

int
rra_log_get_level(void)
{
    return current_log_level;
}

int
rra_log_get_default_level(void)
{
    return default_log_level;
}

gboolean
rra_logv(int in_level, const char *in_subtype,
         const char *in_filename, int in_linenum,
         const char *in_funcname, const char *in_format,
         va_list in_args)
{
    if (in_level <= current_log_level)
    {
	char *format = g_strdup_printf("%s: %s: %d: %s(): %s",
                                       in_subtype,
                                       in_filename, in_linenum, in_funcname,
                                       in_format);

        g_logv(G_LOG_DOMAIN, in_level, format, in_args);

	g_free(format);

        return TRUE;
    }
    else
    {
	return FALSE;
    }
}

gboolean
rra_log(int in_level, const char *in_subtype,
        const char *in_filename, int in_linenum,
        const char *in_funcname, const char *in_format, ...)
{
    gboolean ret_val;
    va_list ap;
    va_start (ap, in_format);

    ret_val = rra_logv(in_level, in_subtype,
                       in_filename, in_linenum,
                       in_funcname, in_format, ap);
    va_end (ap);
    return ret_val;
}
