/*
 * Authors:     James LewisMoss <jlm@racemi.com>
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

#ifndef ENUM_T1_H
#define ENUM_T1_H 1

#include <rregadmin/util/macros.h>

G_BEGIN_DECLS

typedef enum
{
    A_VAL_FOO,
    A_VAL_BAR,
    A_VAL_BAZ,
    A_VAL_BEE,
    A_VAL_BOO,
    A_VAL_DOO,
    A_VAL_BLAH,
    A_VAL_BLEH,
    A_VAL_BUT,
    A_VAL_AND,
} AType1;

typedef enum
{
    B_VAL_ZERO,
    B_VAL_ONE,
} BType1;

G_END_DECLS

#endif // ENUM_T1_H
