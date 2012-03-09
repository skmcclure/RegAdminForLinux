/*
 * Authors:     Sean Loaring
 *              Petter Nordahl-Hagen
 *              James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi Inc
 * Copyright (c) 2005-2006 Sean Loaring
 * Copyright (c) 1997-2004 Petter Nordahl-Hagen
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

#ifndef RREGADMIN_UTIL_ICONV_HELPER_H
#define RREGADMIN_UTIL_ICONV_HELPER_H

#include <iconv.h>

#include <glib.h>

#include <rregadmin/util/macros.h>

G_BEGIN_DECLS

/**
 * @defgroup iconv_helper_util_group Iconv Helper Object
 *
 * @ingroup util_group
 */

/** Object to handle iconv.
 *
 * @ingroup iconv_helper_util_group
 */
typedef struct iconv_helper_ iconv_helper;

/** Get a new iconv_helper.
 *
 * @ingroup iconv_helper_util_group
 */
iconv_helper* iconv_helper_new (void);

/** Free an iconv_helper.
 *
 * @ingroup iconv_helper_util_group
 */
gboolean iconv_helper_free (iconv_helper *in_helper);


/** Get a unique iconv_t for the specified to and from codings.
 *
 * @ingroup iconv_helper_util_group
 */
iconv_t iconv_helper_get(iconv_helper *in_helper,
                         const char *in_to_coding,
                         const char *in_from_coding);

/** Release an iconv_t back to the pool.
 *
 * @ingroup iconv_helper_util_group
 */
gboolean iconv_helper_release (iconv_helper *in_helper, iconv_t in_it,
                               const char *in_to_coding,
                               const char *in_from_coding);


G_END_DECLS

#endif /* RREGADMIN_UTIL_ICONV_HELPER_H */
