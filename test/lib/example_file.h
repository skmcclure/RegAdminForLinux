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

#ifndef RREGADMIN_TEST_SHARED_FILES_H
#define RREGADMIN_TEST_SHARED_FILES_H 1

#include <glib.h>

G_BEGIN_DECLS

typedef enum
{
    EF_KEY,
    EF_VALUE,
    EF_UNKNOWN
} ef_type;

struct ef_line_
{
    ef_type type;
    char *path;
    char *name;
};
typedef struct ef_line_ ef_line;

typedef struct example_file_ example_file;

example_file* ef_open(const char *in_filename);
example_file* ef_new(const char *in_filename, ef_type in_type);

gboolean ef_free(example_file *in_ef);

gboolean ef_line_free(ef_line *in_line);

ef_line* ef_next(example_file *in_ef);
gboolean ef_write_line(example_file *in_ef, const ef_line *in_line);

G_END_DECLS

#endif // RREGADMIN_TEST_SHARED_FILES_H
