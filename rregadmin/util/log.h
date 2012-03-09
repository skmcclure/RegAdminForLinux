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

#ifndef RREGADMIN_UTIL_LOG_H
#define RREGADMIN_UTIL_LOG_H

#include <stdarg.h>
#include <glib.h>

#include <rregadmin/rra_config.h>
#include <rregadmin/util/macros.h>

G_BEGIN_DECLS

/**
 * @defgroup log_util_group Logging Facilities
 *
 * @ingroup util_group
 */


#if !defined(RRA_LOG_SUBTYPE)
#define RRA_LOG_SUBTYPE "Util"
#endif

#define RRA_LOG_LEVEL_ERROR 1 << 2
#define RRA_LOG_LEVEL_CRITICAL 1 << 3
#define RRA_LOG_LEVEL_WARNING 1 << 4
#define RRA_LOG_LEVEL_MESSAGE 1 << 5
#define RRA_LOG_LEVEL_INFO 1 << 6
#define RRA_LOG_LEVEL_DEBUG 1 << 7
#define RRA_LOG_LEVEL_TRACE 1 << 8

extern const int rra_comp_loglevel;

/** Initialize the logging subsystem.
 *
 * @ingroup log_util_group
 */
void rra_log_initialize(void);

/** Set the level of output for the log subsystem.
 *
 * @ingroup log_util_group
 */
gboolean rra_log_set_level(int in_level);

/** Get the current output level for the log subsystem.
 *
 * @ingroup log_util_group
 */
int rra_log_get_level(void);

/** Get the default log level.
 *
 * @ingroup log_util_group
 */
int rra_log_get_default_level(void);

/** Convert string to level.
 *
 * @ingroup log_util_group
 */
int rra_log_string_to_level(const char *in_str);

/** Check to see if the given log level (and above) is supported.
 *
 * @ingroup log_util_group
 */
gboolean rra_log_is_supported_level (int in_level);

/** Log a message.
 *
 * @ingroup log_util_group
 */
gboolean rra_log(int in_level, const char *in_subtype,
                 const char *in_filename, int in_linenum,
                 const char *in_funcname, const char *in_format, ...);

/** Log a message.
 *
 * @ingroup log_util_group
 */
gboolean rra_logv(int in_level, const char *in_subtype,
                  const char *in_filename, int in_linenum,
                  const char *in_funcname, const char *in_format,
                  va_list in_args);

#define RRA_IGNORE_DO_SOMETHING FALSE
/* #define RRA_IGNORE_COMMAND (RRA_IGNORE_DO_SOMETHING ? 1 : 0) */
#define RRA_IGNORE_COMMAND (FALSE)
#define CHECK_COMPILED_LOG_LEVEL(lvl) (RRA_COMPILED_LOG_LEVEL >= lvl)
#define RRA_PRETTY_FUNCTION __func__

#if defined(G_HAVE_ISO_VARARGS)

#if CHECK_COMPILED_LOG_LEVEL(RRA_LOG_LEVEL_WARNING)
/** Output a warning message.
 *
 * @ingroup log_util_group
 */
#define rra_warning(...)                                                \
    rra_log(G_LOG_LEVEL_WARNING, RRA_LOG_SUBTYPE,                       \
            __FILE__, __LINE__, RRA_PRETTY_FUNCTION, __VA_ARGS__)
#else
#define rra_warning(...) RRA_IGNORE_COMMAND
#endif

#if CHECK_COMPILED_LOG_LEVEL(RRA_LOG_LEVEL_MESSAGE)
/** Output a log message.
 *
 * @ingroup log_util_group
 */
#define rra_message(...)                                                \
    rra_log(G_LOG_LEVEL_MESSAGE, RRA_LOG_SUBTYPE,                       \
            __FILE__, __LINE__, RRA_PRETTY_FUNCTION, __VA_ARGS__)
#else
#define rra_message(...) RRA_IGNORE_COMMAND
#endif

#if CHECK_COMPILED_LOG_LEVEL(RRA_LOG_LEVEL_INFO)
/** Output an info message.
 *
 * @ingroup log_util_group
 */
#define rra_info(...)                                                   \
    rra_log(G_LOG_LEVEL_INFO, RRA_LOG_SUBTYPE,                          \
            __FILE__, __LINE__, RRA_PRETTY_FUNCTION, __VA_ARGS__)
#else
#define rra_info(...) RRA_IGNORE_COMMAND
#endif

#if CHECK_COMPILED_LOG_LEVEL(RRA_LOG_LEVEL_DEBUG)
/** Output a debug message.
 *
 * @ingroup log_util_group
 */
#define rra_debug(...)                                                  \
    rra_log(G_LOG_LEVEL_DEBUG, RRA_LOG_SUBTYPE,                         \
            __FILE__, __LINE__, RRA_PRETTY_FUNCTION, __VA_ARGS__)
#else
#define rra_debug(...) RRA_IGNORE_COMMAND
#endif

#if CHECK_COMPILED_LOG_LEVEL(RRA_LOG_LEVEL_TRACE)
/** Output a trace message.
 *
 * @ingroup log_util_group
 */
#define rra_trace(...)                                                  \
    rra_log(RRA_LOG_LEVEL_TRACE, RRA_LOG_SUBTYPE,                       \
            __FILE__, __LINE__, RRA_PRETTY_FUNCTION, __VA_ARGS__)
#else
#define rra_trace(...) RRA_IGNORE_COMMAND
#endif


#elif defined(G_HAVE_GNUC_VARARGS)

#if CHECK_COMPILED_LOG_LEVEL(RRA_LOG_LEVEL_WARNING)
#define rra_warning(format...)                                          \
    rra_log(G_LOG_LEVEL_WARNING, RRA_LOG_SUBTYPE,                       \
            __FILE__, __LINE__, RRA_PRETTY_FUNCTION, format)
#else
#define rra_warning(...) RRA_IGNORE_COMMAND
#endif

#if CHECK_COMPILED_LOG_LEVEL(RRA_LOG_LEVEL_MESSAGE)
#define rra_message(format...)                                          \
    rra_log(G_LOG_LEVEL_MESSAGE, RRA_LOG_SUBTYPE,                       \
            __FILE__, __LINE__, RRA_PRETTY_FUNCTION, format)
#else
#define rra_message(...) RRA_IGNORE_COMMAND
#endif


#if CHECK_COMPILED_LOG_LEVEL(RRA_LOG_LEVEL_INFO)
#define rra_info(format...)                                             \
    rra_log(G_LOG_LEVEL_INFO, RRA_LOG_SUBTYPE,                          \
            __FILE__, __LINE__, RRA_PRETTY_FUNCTION, format)
#else
#define rra_info(...) RRA_IGNORE_COMMAND
#endif


#if CHECK_COMPILED_LOG_LEVEL(RRA_LOG_LEVEL_DEBUG)
#define rra_debug(format...)                                            \
    rra_log(G_LOG_LEVEL_DEBUG, RRA_LOG_SUBTYPE,                         \
            __FILE__, __LINE__, RRA_PRETTY_FUNCTION, format)
#else
#define rra_debug(...) RRA_IGNORE_COMMAND
#endif


#if CHECK_COMPILED_LOG_LEVEL(RRA_LOG_LEVEL_TRACE)
#define rra_trace(format...)                                            \
    rra_log(RRA_LOG_LEVEL_TRACE, RRA_LOG_SUBTYPE,                       \
            __FILE__, __LINE__, RRA_PRETTY_FUNCTION, format)
#else
#define rra_trace(...) RRA_IGNORE_COMMAND
#endif

#else

static gboolean
rra_warning(const char *in_format, ...)
{
    if (CHECK_COMPILED_LOG_LEVEL(RRA_LOG_LEVEL_WARNING))
    {
        va_list args;
        va_start (args, in_format);
        rra_logv (G_LOG_LEVEL_WARNING, RRA_LOG_SUBTYPE,
                  __FILE__, 0, "", format, args);
        va_end (args);
    }
}
static gboolean
rra_message(const char *in_format, ...)
{
    if (CHECK_COMPILED_LOG_LEVEL(RRA_LOG_LEVEL_MESSAGE))
    {
        va_list args;
        va_start (args, in_format);
        rra_logv (G_LOG_LEVEL_MESSAGE, RRA_LOG_SUBTYPE,
                  __FILE__, 0, "", format, args);
        va_end (args);
    }
}
static gboolean
rra_info(const char *in_format, ...)
{
    if (CHECK_COMPILED_LOG_LEVEL(RRA_LOG_LEVEL_INFO))
    {
        va_list args;
        va_start (args, in_format);
        rra_logv (G_LOG_LEVEL_INFO, RRA_LOG_SUBTYPE,
                  __FILE__, 0, "", format, args);
        va_end (args);
    }
}
static gboolean
rra_debug(const char *in_format, ...)
{
    if (CHECK_COMPILED_LOG_LEVEL(RRA_LOG_LEVEL_DEBUG))
    {
        va_list args;
        va_start (args, in_format);
        rra_logv (G_LOG_LEVEL_DEBUG, RRA_LOG_SUBTYPE,
                  __FILE__, 0, "", format, args);
        va_end (args);
    }
}
static gboolean
rra_trace(const char *in_format, ...)
{
    if (CHECK_COMPILED_LOG_LEVEL(RRA_LOG_LEVEL_TRACE))
    {
        va_list args;
        va_start (args, in_format);
        rra_logv (RRA_LOG_LEVEL_TRACE, RRA_LOG_SUBTYPE,
                  __FILE__, 0, "", format, args);
        va_end (args);
    }
}

#endif

#undef CHECK_COMPILED_LOG_LEVEL

G_END_DECLS

#endif /* RREGADMIN_UTIL_LOG_H */
