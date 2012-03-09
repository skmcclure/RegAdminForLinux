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

#include <rregadmin/hive/defines.h>

int
rrafs_statfs(const char *path, struct statvfs *stbuf)
{
    if (path == NULL
        || stbuf == NULL)
    {
        return -EINVAL;
    }

    stbuf->f_bsize = HIVE_PAGE_LEN;
    stbuf->f_frsize = 8;

    /** @todo fix the next 4 values. need registry_get_stats func */
    stbuf->f_blocks = 0; /* size in bytes */
    stbuf->f_bfree = 0; /* free blocks */
    stbuf->f_bavail = 0; /* free blocks for non-root */

    stbuf->f_files = 0;
    stbuf->f_ffree = 0;
    stbuf->f_favail = 0;

    /* stbuf->f_fsid = 0; */
    /* stbuf->f_flag = 0; */

    stbuf->f_namemax = 512;

    return 0;
}
