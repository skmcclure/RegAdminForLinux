/*
  Copyright 2007 Racemi Inc
  Copyright (C) Andrew Tridgell 		2004

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <rregadmin/util/flag_map.h>

gboolean
flag_map_names_to_flags(const FlagMap *in_map,
                        const char **in_str,
                        fm_flag_type *in_flags,
                        char in_end_char,
                        gboolean in_reset_on_error)
{
    fm_flag_type orig_flags = *in_flags;
    const char *orig_str = *in_str;

    gboolean ret_val = TRUE;

    while (*in_str[0] != '\0')
    {
        if (*in_str[0] == in_end_char)
        {
            break;
        }

        int i;
        for (i = 0; in_map[i].name; i++)
        {
            size_t l = strlen(in_map[i].name);
            if (strncmp(in_map[i].name, *in_str, l) == 0)
            {
                *in_flags |= in_map[i].flag;
                *in_str += l;
                break;
            }
        }
        if (in_map[i].name == NULL)
        {
            ret_val = FALSE;
            break;
        }
    }

    if (*in_str[0] != in_end_char)
    {
        ret_val = FALSE;
    }

    if (in_reset_on_error && ret_val == FALSE)
    {
        *in_flags = orig_flags;
        *in_str = orig_str;
    }

    return ret_val;
}

gboolean
flag_map_flags_to_names(const FlagMap *in_map,
                        fm_flag_type *in_flags,
                        ustring *in_ustr,
                        gboolean in_check_all)
{
    int orig_flags = *in_flags;
    int init_size;
    int i;

    init_size = ustr_size(in_ustr);

    /* try to find an exact match */
    for (i = 0; in_map[i].name; i++)
    {
        if (in_map[i].flag == *in_flags)
        {
            *in_flags &= ~in_map[i].flag;
            ustr_printfa(in_ustr, "%s", in_map[i].name);
            return TRUE;
        }
    }

    /* now by bits */
    for (i = 0; in_map[i].name && *in_flags != 0; i++)
    {
        if ((*in_flags & in_map[i].flag) == in_map[i].flag)
        {
            ustr_printfa(in_ustr, "%s", in_map[i].name);
            *in_flags &= ~in_map[i].flag;
        }
    }

    /*
     * If we don't map all the bits then remove what we added from
     * the string.
     */
    if (in_check_all && *in_flags != 0)
    {
        int trim_amount = ustr_size(in_ustr) - init_size;
        ustr_trim_back(in_ustr, trim_amount);
        *in_flags = orig_flags;
        return FALSE;
    }

    return TRUE;
}
