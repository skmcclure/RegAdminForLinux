/*
 * Authors:	James LewisMoss <jlm@racemi.com>
 *
 * Copyright 2006,2008 Racemi Inc
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


#ifndef RREGADMIN_UTIL_MALLOC_H
#define RREGADMIN_UTIL_MALLOC_H 1

#include <glib.h>

#include <rregadmin/rra_config.h>
#include <rregadmin/util/macros.h>

G_BEGIN_DECLS

#ifdef HAVE_G_SLICE_ALLOC
#define USE_GLIB_SLICES 1
#endif

/* #ifdef RRA_INCLUDE_DEBUG_CODE */
#define RRA_USE_ZEROING_ALLOCATORS
/* #endif */

#ifdef USE_GLIB_SLICES

#  ifdef RRA_USE_ZEROING_ALLOCATORS
#    define rra_new_type(t) g_slice_new0(t)
#  else
#    define rra_new_type(t) g_slice_new(t)
#  endif

#  define rra_free_type(t, block) g_slice_free(t, block)

#else

#  ifdef RRA_USE_ZEROING_ALLOCATORS
#    define rra_new_type(t) g_new0(t, 1)
#  else
#    define rra_new_type(t) g_new(t, 1)
#  endif

#  define rra_free_type(t, block) g_free(block)

#endif

#ifdef RRA_USE_ZEROING_ALLOCATORS
#  define rra_new(s) g_malloc0(s)
#  define rra_new_sizeof(t) (t*)g_malloc0(sizeof(t))
#else
#  define rra_new(s) g_malloc(s)
#  define rra_new_sizeof(t) (t*)g_malloc(sizeof(t))
#endif

#define rra_free(s, block) g_free((gpointer)block)

G_END_DECLS

#endif // RREGADMIN_UTIL_MALLOC_H

/*
 * Local variables:
 * mode: c++
 * End:
 */
