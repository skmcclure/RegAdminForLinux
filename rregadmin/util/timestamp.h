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

#ifndef RREGADMIN_UTIL_TIMESTAMP_H
#define RREGADMIN_UTIL_TIMESTAMP_H

#include <sys/time.h>
#include <time.h>

#include <glib.h>
#include <rregadmin/util/macros.h>
#include <rregadmin/util/ustring.h>

G_BEGIN_DECLS

/**
 * @defgroup nttime_util_group NTTIME Handling Routines
 *
 * @ingroup util_group
 */

/** Number of 100s of nanoseconds from the begining of the year 1601 AD
 * to the time represented.
 *
 * @see cifs6.txt: section 3.5, page 30
 *
 * @ingroup nttime_util_group
 */
typedef gint64 NTTIME;

/** typedef for a type to contain nanoseconds.
 *
 * @ingroup nttime_util_group
 */
typedef guint32 nanosecs;

/** Create an NTTIME for now.
 *
 * @ingroup nttime_util_group
 */
NTTIME nttime_now(void);

/** Get an error NTTIME.
 *
 * @ingroup nttime_util_group
 */
NTTIME nttime_get_error(void);

/** Get the largest non-relative NTTIME.
 *
 * @ingroup nttime_util_group
 */
NTTIME nttime_get_largest_date(void);

/** Create an NTTIME using the data from in_tm and in_ns.
 *
 * @ingroup nttime_util_group
 */
NTTIME nttime_from_tm(const struct tm *in_tm, nanosecs in_ns);

/** Create an NTTIME from a time_t.
 *
 * @ingroup nttime_util_group
 */
NTTIME nttime_from_time_t(time_t in_tt, nanosecs in_ns);

/** Convert an NTTIME into a struct tm and nanoseconds.
 *
 * @ingroup nttime_util_group
 */
gboolean nttime_to_tm(NTTIME in_time, struct tm *out_tm, nanosecs *out_nano);

/** Convert an NTTIME into a time_t and nanoseconds.
 *
 * @ingroup nttime_util_group
 */
gboolean nttime_to_time_t(NTTIME in_time, time_t *out_tt, nanosecs *out_nano);

/** Create a string representing the NTTIME.
 *
 * @ingroup nttime_util_group
 */
ustring* nttime_to_str(NTTIME in_time);

/** Is this an error NTTIME?
 *
 * @ingroup nttime_util_group
 */
gboolean nttime_is_error(NTTIME in_time);

/** Is the NTTIME a relative time.
 *
 * @ingroup nttime_util_group
 */
gboolean nttime_is_relative(NTTIME in_time);

/** Get the year of the NTTIME.
 *
 * @ingroup nttime_util_group
 */
int nttime_year(NTTIME in_time);

/** Get the month of the NTTIME.
 *
 * This is zero based so January == 0, February == 1, etc
 *
 * @ingroup nttime_util_group
 */
int nttime_month(NTTIME in_time);

/** Get the day of the month of the NTTIME.
 *
 * @ingroup nttime_util_group
 */
int nttime_month_day(NTTIME in_time);

/** Get the day of the year of the NTTIME.
 *
 * @ingroup nttime_util_group
 */
int nttime_year_day(NTTIME in_time);

/** Get the hour of the NTTIME.
 *
 * @ingroup nttime_util_group
 */
int nttime_hour(NTTIME in_time);

/** Get the minute of the NTTIME.
 *
 * @ingroup nttime_util_group
 */
int nttime_minute(NTTIME in_time);

/** Get the second of the NTTIME.
 *
 * @ingroup nttime_util_group
 */
int nttime_second(NTTIME in_time);

/** Get the nanosecond of the NTTIME.
 *
 * @ingroup nttime_util_group
 */
nanosecs nttime_nanosecond(NTTIME in_time);


G_END_DECLS

#endif /* RREGADMIN_UTIL_TIMESTAMP_H */
