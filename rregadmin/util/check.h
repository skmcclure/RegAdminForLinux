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

#ifndef RREGADMIN_UTIL_CHECK_H
#define RREGADMIN_UTIL_CHECK_H 1

#include <rregadmin/util/macros.h>

#include <stdarg.h>

#include <glib.h>

G_BEGIN_DECLS

/** @defgroup check_util_group Functions and structures to write checks.
 *
 * @ingroup util_group
 */

/* Verbosity levels for check messages.
 *
 * @ingroup check_util_group
 */
typedef enum
{
    RRA_CHECK_VERBOSITY_QUIET,
    RRA_CHECK_VERBOSITY_ERROR,
    RRA_CHECK_VERBOSITY_WARNING,
    RRA_CHECK_VERBOSITY_MESSAGE,
    RRA_CHECK_VERBOSITY_INFO,
    RRA_CHECK_VERBOSITY_DEBUG
} RRACheckVerbosity;

/* Check flag type.
 *
 * @ingroup check_util_group
 */
typedef guint32 rra_check_flags;

/* Check flag locations
 *
 * @ingroup check_util_group
 */
typedef enum
{
    RRA_CHECK_FLAG_REPAIR = 1,
} RRACheckFlagLocation;

/* Data for check functions.
 *
 * @ingroup check_util_group
 */
struct RRACheckData_
{
    RRACheckVerbosity verbosity;
    int level;
    rra_check_flags flags;
};
typedef struct RRACheckData_ RRACheckData;

void rra_check_logv(RRACheckData *in_data, RRACheckVerbosity in_verbosity,
                    const char *in_format, va_list in_args);

void rra_check_log(RRACheckData *in_data, RRACheckVerbosity in_verbosity,
                   const char *in_format, ...);

#define rra_check_error(in_data, ...)                               \
    do                                                              \
    {                                                               \
        if (in_data->verbosity >= RRA_CHECK_VERBOSITY_ERROR)        \
        {                                                           \
            rra_check_log(in_data, RRA_CHECK_VERBOSITY_ERROR,       \
                          __VA_ARGS__);                             \
        }                                                           \
    } while(0)

#define rra_check_warning(in_data, ...)                             \
    do                                                              \
    {                                                               \
        if (in_data->verbosity >= RRA_CHECK_VERBOSITY_WARNING)      \
        {                                                           \
            rra_check_log(in_data, RRA_CHECK_VERBOSITY_WARNING,     \
                          __VA_ARGS__);                             \
        }                                                           \
    } while(0)

#define rra_check_message(in_data, ...)                             \
    do                                                              \
    {                                                               \
        if (in_data->verbosity >= RRA_CHECK_VERBOSITY_MESSAGE)      \
        {                                                           \
            rra_check_log(in_data, RRA_CHECK_VERBOSITY_MESSAGE,     \
                          __VA_ARGS__);                             \
        }                                                           \
    } while(0)

#define rra_check_info(in_data, ...)                                  \
    do                                                                \
    {                                                                 \
        if (in_data->verbosity >= RRA_CHECK_VERBOSITY_INFO)           \
        {                                                             \
            rra_check_log(in_data, RRA_CHECK_VERBOSITY_INFO,          \
                          __VA_ARGS__);                               \
        }                                                             \
    } while(0)

#define rra_check_debug(in_data, ...)                                 \
    do                                                                \
    {                                                                 \
        if (in_data->verbosity >= RRA_CHECK_VERBOSITY_DEBUG)          \
        {                                                             \
            rra_check_log(in_data, RRA_CHECK_VERBOSITY_DEBUG,         \
                          __VA_ARGS__);                               \
        }                                                             \
    } while(0)

#define rra_check_checking(in_data, ...)                            \
    do                                                              \
    {                                                               \
        gchar *msg = g_strdup_printf(__VA_ARGS__);                  \
        rra_check_info(in_data, _("Checking %s"), msg);    \
        g_free(msg);                                                \
    } while(0)


G_END_DECLS

#endif // RREGADMIN_UTIL_CHECK_H
