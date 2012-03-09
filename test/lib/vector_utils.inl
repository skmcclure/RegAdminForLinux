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

#ifndef RREGADMIN_TEST_VECTOR_UTILS_INL
#define RREGADMIN_TEST_VECTOR_UTILS_INL

#include <vector>

template <typename TypeT, typename CharT, typename TraitsT,
          template <typename CharT, typename TraitsT> class OstreamT>
          OstreamT<CharT, TraitsT>&
          operator<<(OstreamT<CharT, TraitsT>& in_ostream,
                     const std::vector<TypeT>& in_vec)
{
    if (in_vec.size() == 0)
    {
        return in_ostream;
    }
    else if(in_vec.size() == 1)
    {
        in_ostream << in_vec[0];
        return in_ostream;
    }
    else
    {
        typename std::vector<TypeT>::const_iterator i
            = in_vec.begin();
        in_ostream << *i;
        i++;
        for (; i != in_vec.end(); i++)
        {
            in_ostream << "," << *i;
        }
        return in_ostream;
    }
}

#endif // RREGADMIN_TEST_VECTOR_UTILS_INL

// Local variables:
// mode: c++
// End:
