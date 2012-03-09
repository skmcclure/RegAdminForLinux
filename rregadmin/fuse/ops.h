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

#ifndef RREADMIN_FUSE_OPS_H
#define RREADMIN_FUSE_OPS_H 1

#define FUSE_USE_VERSION 26

#include <rregadmin/rra_config.h>

#include <fuse.h>

#include <rregadmin/basic.h>
#include <rregadmin/util/macros.h>

G_BEGIN_DECLS

struct rrafs_data
{
    RRACliState *state;
    const char *regdir;
    uid_t mounter_uid;
    gid_t mounter_gid;
};

#if FUSE_VERSION >= 23
#define RRAFS_USE_INIT 1
#if FUSE_VERSION >= 26
void *rrafs_init(struct fuse_conn_info *conn);
#else
void *rrafs_init(void);
#endif
#endif

const struct fuse_operations* rrafs_get_operations(void);

struct rrafs_data* rrafs_get_rrafs_data(void);

int rrafs_getattr(const char *path, struct stat *stbuf);

int rrafs_readlink(const char *path, char *buf, size_t size);

/* Deprecated: int (*getdir) (const char *, fuse_dirh_t, fuse_dirfil_t); */

int rrafs_mknod(const char *path, mode_t mode, dev_t rdev);

int rrafs_mkdir(const char *path, mode_t mode);

int rrafs_unlink(const char *path);

int rrafs_rmdir(const char *path);

int rrafs_symlink(const char *from, const char *to);

int rrafs_rename(const char *from, const char *to);

int rrafs_link(const char *from, const char *to);

int rrafs_chmod(const char *path, mode_t mode);

int rrafs_chown(const char *path, uid_t uid, gid_t gid);

int rrafs_truncate(const char *path, off_t size);

/* Deprecated: int (*utime) (const char *, struct utimbuf *); */

int rrafs_open(const char *path, struct fuse_file_info *fi);

int rrafs_read(const char *path, char *buf, size_t size, off_t offset,
               struct fuse_file_info *fi);

int rrafs_write(const char *path, const char *buf, size_t size,
                off_t offset, struct fuse_file_info *fi);

int rrafs_statfs(const char *path, struct statvfs *stbuf);

/* int rrafs_flush(consh char *path, struct fuse_file_info *fi); */

int rrafs_release(const char *path, struct fuse_file_info *fi);

int rrafs_fsync(const char *path, int isdatasync,
                struct fuse_file_info *fi);

#ifdef RRAFS_HAVE_SETXATTR

/* xattr operations are optional and can safely be left unimplemented */
int rrafs_setxattr(const char *path, const char *name, const char *value,
                   size_t size, int flags);

int rrafs_getxattr(const char *path, const char *name, char *value,
                   size_t size);

int rrafs_listxattr(const char *path, char *list, size_t size);

int rrafs_removexattr(const char *path, const char *name);

#endif /* HAVE_SETXATTR */

/* int rrafs_opendir (const char *, struct fuse_file_info *); */

int rrafs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                  off_t offset, struct fuse_file_info *fi);

/* int rrafs_releasedir (const char *, struct fuse_file_info *); */

/* int rrafs_fsyncdir (const char *, int, struct fuse_file_info *); */

/* void *rrafs_init (struct fuse_conn_info *conn); */

/* void rrafs_destroy (void *); */

int rrafs_access(const char *path, int mask);

/* int rrafs_create (const char *, mode_t, struct fuse_file_info *); */

/* int rrafs_ftruncate (const char *, off_t, struct fuse_file_info *); */

/* int rrafs_fgetattr (const char *, struct stat *, struct fuse_file_info *); */

/* int rrafs_lock (const char *, struct fuse_file_info *,
   int cmd, struct flock *); */

int rrafs_utimens(const char *path, const struct timespec ts[2]);

/* int rrafs_bmap (const char *, size_t blocksize, uint64_t *idx); */

G_END_DECLS

#endif /* RREADMIN_FUSE_OPS_H */
