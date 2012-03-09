/*
 *  Copyright 2008 Racemi Inc
 *        Written by: James LewisMoss <jlm@racemi.com> or
 *        <jim@lewismoss.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef RREGADMIN_TEST_VECTOR_UTILS_H
#define RREGADMIN_TEST_VECTOR_UTILS_H 1

#include <vector>

/** Output a vector to a stream.
 */
template <typename TypeT, typename CharT, typename TraitsT,
          template <typename CharT, typename TraitsT> class OstreamT>
          OstreamT<CharT, TraitsT>&
          operator<<(OstreamT<CharT, TraitsT>&,
                     const std::vector<TypeT>&);


#include "vector_utils.inl"

#endif // RREGADMIN_TEST_VECTOR_UTILS_H

// Local variables:
// mode: c++
// End:
