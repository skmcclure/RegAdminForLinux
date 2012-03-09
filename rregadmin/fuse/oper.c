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

static struct fuse_operations rrafs_opers = {

#ifdef RRAFS_USE_INIT
    .init        = rrafs_init,
    /* .destroy */
#endif

    .getattr     = rrafs_getattr,
    .readlink    = rrafs_readlink,
    /* .getdir Deprecated */
    .mknod       = rrafs_mknod,
    .mkdir       = rrafs_mkdir,
    .unlink      = rrafs_unlink,
    .rmdir       = rrafs_rmdir,
    .symlink     = rrafs_symlink,
    .rename      = rrafs_rename,
    .link        = rrafs_link,
    .chmod       = rrafs_chmod,
    .chown       = rrafs_chown,
    .truncate    = rrafs_truncate,
    /* .utime Deprecated */
    .open        = rrafs_open,
    .read        = rrafs_read,
    .write       = rrafs_write,
    .statfs      = rrafs_statfs,
    /* .flush */
    .release     = rrafs_release,
    .fsync       = rrafs_fsync,

#ifdef HAVE_SETXATTR
    .setxattr    = rrafs_setxattr,
    .getxattr    = rrafs_getxattr,
    .listxattr   = rrafs_listxattr,
    .removexattr = rrafs_removexattr,
#endif

    /* .opendir */
    .readdir     = rrafs_readdir,
    /* .releasedir */
    /* .fsyncdir */
    .access      = rrafs_access,

#if FUSE_VERSION >= 25
    /* .create */
    /* .ftruncate */
    /* .fgetattr */
#endif

    /* .lock */
    .utimens     = rrafs_utimens,
    /* .bmap */
};

const struct fuse_operations*
rrafs_get_operations(void)
{
    return &rrafs_opers;
}
