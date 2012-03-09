/*
 * Authors:     James LewisMoss <jlm@racemi.com>
 *
 * Copyright 2007 Racemi Inc
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
 */

#include "ops.h"

#include <errno.h>

#ifdef RRAFS_USE_INIT

struct fuse_conn_info *my_ci = NULL;

void *rrafs_init(
#if FUSE_VERSION >= 26
    struct fuse_conn_info *conn
#else
    void
#endif
)
{
#if FUSE_VERSION >= 26
    my_ci = conn;
#endif
    return NULL;
}

#endif
