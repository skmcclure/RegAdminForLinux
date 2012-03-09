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

#include <t2.h>

#include <stdio.h>
#include <glib.h>

struct t2_
{
    int val;
};

t2*
t2_new(int in_val)
{
    t2* ret_val = g_new0(t2, 1);
    ret_val->val = in_val;
    return ret_val;
}

void
t2_free(t2* in_t2)
{
    g_free(in_t2);
}

int
t2_get(t2* in_t2)
{
    return in_t2->val;
}

void
t2_set(t2 *in_t2, int in_new_val)
{
    in_t2->val = in_new_val;
}

void
print_t2(t2 *in_t2)
{
    printf("t2(%d)", in_t2->val);
}

void
t2_bogus(int in_val)
{
    printf("bogus -> %d", in_val);
}
