/*
 * Authors:        James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2007 Racemi Inc
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

#ifndef RREGADMIN_SAM_TEST_USERXINFO_H
#define RREGADMIN_SAM_TEST_USERXINFO_H 1

#include <rregadmin/registry/types.h>
#include <rregadmin/util/macros.h>

G_BEGIN_DECLS

#define SAM_PATH_PREFIX "\\HKEY_LOCAL_MACHINE\\SAM\\SAM"
#define SAM_TEST_USER_PATH SAM_PATH_PREFIX "\\Domains\\Account\\Users"
/* This should be the Admininstrator account */
#define SAM_TEST_ADMIN_PATH SAM_TEST_USER_PATH "\\000001F4"

G_END_DECLS

#endif /* RREGADMIN_SAM_TEST_USERXINFO_H */
