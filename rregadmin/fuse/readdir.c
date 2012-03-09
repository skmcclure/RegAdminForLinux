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

int
rrafs_readdir(const char *path, void DECLARE_UNUSED(*buf),
              fuse_fill_dir_t DECLARE_UNUSED(filler),
              off_t DECLARE_UNUSED(offset),
              struct fuse_file_info DECLARE_UNUSED(*fi))
{
    if (path == NULL)
    {
        return -EINVAL;
    }

    return -ENOTSUP;
}
