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

#ifndef T2_H
#define T2_H 1

#include <rregadmin/util/macros.h>

G_BEGIN_DECLS

typedef struct t2_ t2;

t2* t2_new(int in_val);
void t2_free(t2* in_t2);

int t2_get(t2* in_t2);
void t2_set(t2 *in_t2, int in_new_val);

void print_t2(t2 *in_t2);
void t2_bogus(int in_val);

G_END_DECLS

#endif // T2_H
