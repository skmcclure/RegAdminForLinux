/*
 * Authors:     Sean Loaring
 *              Petter Nordahl-Hagen
 *              James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi Inc
 * Copyright (c) 2005-2006 Sean Loaring
 * Copyright (c) 1997-2004 Petter Nordahl-Hagen
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

#include <rregadmin/config.h>

#include <string.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/util/value_type.h>

gboolean
value_type_is_valid(ValueType type)
{
    if (type <= REG_MAX)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

const char *valstr[] = {
    "NONE",
    "SZ",
    "EXPAND_SZ",
    "BINARY",
    "DWORD",
    "DWORD_BIG_ENDIAN",
    "LINK",
    "MULTI_SZ",
    "RESOURCE_LIST",
    "FULL_RESOURCE_DESCRIPTOR",
    "RESOURCE_REQUIREMENTS_LIST",
    "QWORD",
    ""
};

const char *long_valstr[] = {
    "REG_NONE",
    "REG_SZ",
    "REG_EXPAND_SZ",
    "REG_BINARY",
    "REG_DWORD",
    "REG_DWORD_BIG_ENDIAN",
    "REG_LINK",
    "REG_MULTI_SZ",
    "REG_RESOURCE_LIST",
    "REG_FULL_RESOURCE_DESCRIPTOR",
    "REG_RESOURCE_REQUIREMENTS_LIST",
    "REG_QWORD",
    ""
};

const char *descrip_valstr[] = {
    N_("No associated type (data entered in hex)"),
    N_("String"),
    N_("String with %ENVVAR%"),
    N_("Binary"),
    N_("Little Endian dword"),
    N_("Big Endian dword"),
    N_("Symbolic link stored in UTF16"),
    N_("Multiple strings"),
    N_("Unknown"),
    N_("Unknown"),
    N_("Unknown"),
    N_("Little Endian qword"),
    ""
};


ValueType
value_type_from_string (const char *tstr)
{
    int i;
    const char *s = tstr;

    if (0 == strncmp (s, "REG_", 4))
    {
        s = s + 4;
    }

    for (i = 0; i <= REG_MAX; i++)
    {
        if (0 == strcmp (s, valstr[i]))
            return i;
    }

    return -1;
}

const char *
value_type_get_string (ValueType type)
{
    if (type <= REG_MAX)
    {
        return valstr[type];
    }
    else
    {
        return "(unknown)";
    }
}

const char *
value_type_get_description (ValueType type)
{
    if (type <= REG_MAX)
    {
        return _(descrip_valstr[type]);
    }
    else
    {
        return _("Unknown type.");
    }
}

gboolean
value_type_is_string(ValueType type)
{
    if (type == REG_SZ
        || type == REG_MULTI_SZ
        || type == REG_LINK
        || type == REG_EXPAND_SZ)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

gboolean
value_type_is_binary(ValueType type)
{
    if (type == REG_BINARY
        || type == REG_RESOURCE_LIST
        || type == REG_FULL_RESOURCE_DESCRIPTOR
        || type == REG_RESOURCE_REQUIREMENTS_LIST
        || type == REG_NONE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

gboolean
value_type_is_integral(ValueType type)
{
    if (type == REG_DWORD
        || type == REG_DWORD_BIG_ENDIAN
        || type == REG_QWORD)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
