/*
 * Authors:	James LewisMoss <jlm@racemi.com>
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

#ifndef RRA_USTRING_TO_STRING_H
#define RRA_USTRING_TO_STRING_H

#include <string>

#include <rregadmin/util/ustring.h>

inline std::string
to_string(const ustring *in_ustr)
{
    return std::string(ustr_as_utf8(in_ustr));
}

#endif // RRA_USTRING_TO_STRING_H

// Local variables:
// mode: c++
// End:
