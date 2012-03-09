/*
 * Authors:	James LewisMoss <jlm@racemi.com>
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

#include <stdio.h>

#include <fstream>

#include "FileInfo.hpp"
#include "sha1.h"

namespace RRegAdmin
{
    namespace Test
    {
        FileInfo::FileInfo(const std::string& in_filename)
        {
            SHA1 sha_gen;

            std::ifstream is1(in_filename.c_str());

            while (is1)
            {
                const int buf_size = 4096;
                char buf[buf_size];

                is1.read(buf, buf_size);

                if (is1.gcount() > 0)
                {
                    sha_gen.addBytes(buf, is1.gcount());
                }
            }

            unsigned char *data = sha_gen.getDigest();

            memcpy(sha_bytes, data, 20);

            free(data);
        }

        const unsigned char *
        FileInfo::get_bytes(void) const
        {
            return sha_bytes;
        }

        bool
        operator==(const FileInfo &in_fi1, const FileInfo &in_fi2)
        {
            const unsigned char *b1 = in_fi1.get_bytes();
            const unsigned char *b2 = in_fi2.get_bytes();

            for (int i = 0; i < 20; i++)
            {
                if (b1[i] != b2[i])
                {
                    return false;
                }
            }
            return true;
        }

        bool
        operator!=(const FileInfo &in_fi1, const FileInfo &in_fi2)
        {
            return !(in_fi1 == in_fi2);
        }

        std::ostream&
        operator<<(std::ostream &in_os, const FileInfo &in_fi)
        {
            const unsigned char *bts = in_fi.get_bytes();

            in_os << "FileInfo(";
            for (int i = 0; i < 20; i++)
            {
                in_os << std::hex << (int)bts[i];
            }
            in_os << ")";

            return in_os;
        }

    }
}
