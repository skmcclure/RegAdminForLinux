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

#include <string>
#include <iosfwd>

namespace RRegAdmin
{
    namespace Test
    {
        class FileInfo
        {
        private:
            unsigned char sha_bytes[20];

        public:
            FileInfo(const std::string& in_filename);

            const unsigned char *get_bytes(void) const;
        };

        bool operator==(const FileInfo &in_fi1, const FileInfo &in_fi2);
        bool operator!=(const FileInfo &in_fi1, const FileInfo &in_fi2);

        std::ostream& operator<<(std::ostream &in_os, const FileInfo &in_fi);
    }
}
