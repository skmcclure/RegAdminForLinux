/*
 * Authors:       James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi Inc
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

#ifndef RREGADMIN_SAM_TYPES_H
#define RREGADMIN_SAM_TYPES_H 1

#include <rregadmin/util/macros.h>
#include <rregadmin/util/types.h>

G_BEGIN_DECLS

typedef struct SamUser_ SamUser;

typedef struct SamInfo_ SamInfo;

/** ACB flags
 *
 * @ingroup userfinfo_sam_group
 */
typedef enum
{
    /* Act disabled */
    ACB_DISABLED   = 0x0001,
    /* Home directory required */
    ACB_HOMDIRREQ  = 0x0002,
    /* User password not req */
    ACB_PWNOTREQ   = 0x0004,
    /* Temporary duplicate account?? */
    ACB_TEMPDUP    = 0x0008,
    /* Normal user account */
    ACB_NORMAL     = 0x0010,
    /* MNS logon user account */
    ACB_MNS        = 0x0020,
    /* Interdomain trust account */
    ACB_DOMTRUST   = 0x0040,
    /* Workstation trust account */
    ACB_WSTRUST    = 0x0080,

    /*  Server trust account */
    ACB_SVRTRUST   = 0x0100,
    /* User password does not expire */
    ACB_PWNOEXP    = 0x0200,
    /* Seems not to be used on failed console logins at least */
    /* Account auto locked */
    ACB_AUTOLOCK   = 0x0400,
} acb_flags;

enum
{
    ACB_ERROR_VALUE = 0xffff,
    COUNTRY_CODE_ERROR_VALUE = 0xffff,
};

typedef guint16 acb_type;

typedef guint16 country_code_type;


G_END_DECLS

#endif /* RREGADMIN_SAM_TYPES_H */
