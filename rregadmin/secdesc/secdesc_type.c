/*
  Copyright 2008 Racemi Inc
  Copyright Andrew Tridgell 2004

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

#include <rregadmin/config.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/secdesc/secdesc_type.h>

#include <rregadmin/secdesc/log.h>

struct acl_flag_conversion
{
    secdesc_type_code code;
    const char *conversion;
};


struct acl_flag_conversion dacl_conversions[] = {
    { SEC_DESC_DACL_AUTO_INHERITED, N_("AutoInherited") },
    { SEC_DESC_DACL_AUTO_INHERIT_REQ, N_("AutoInheritReq") },
    { SEC_DESC_DACL_DEFAULTED, N_("Defaulted") },
    { SEC_DESC_DACL_PROTECTED, N_("Protected") },
    { SEC_DESC_DACL_TRUSTED, N_("Trusted") },
};

struct acl_flag_conversion sacl_conversions[] = {
    { SEC_DESC_SACL_AUTO_INHERITED, N_("AutoInherited") },
    { SEC_DESC_SACL_AUTO_INHERIT_REQ, N_("AutoInheritReq") },
    { SEC_DESC_SACL_DEFAULTED, N_("Defaulted") },
    { SEC_DESC_SACL_PROTECTED, N_("Protected") },
};

static void
add_acl_string(secdesc_type in_type,
               ustring *in_ustr,
               secdesc_type_code in_base_code,
               secdesc_type in_extra_codes,
               struct acl_flag_conversion* in_conversions,
               int in_conversions_len,
               const char *in_start_str)
{
    if ((in_type & (in_base_code | in_extra_codes)) != 0)
    {
        if (ustr_strlen(in_ustr) != 0)
        {
            ustr_strcat(in_ustr, ",");
        }

        if ((in_type & in_extra_codes) != 0)
        {
            ustring *tmp_str = ustr_new();

            int i;
            for (i = 0; i < in_conversions_len; i++)
            {
                if ((in_type & in_conversions[i].code) != 0)
                {
                    if (ustr_strlen(tmp_str) != 0)
                    {
                        ustr_strcat(tmp_str, ",");
                    }
                    ustr_strcat(tmp_str, _(in_conversions[i].conversion));
                }
            }
            ustr_printfa(in_ustr, "%s(%s)", in_start_str,
                         ustr_as_utf8(tmp_str));
            ustr_free(tmp_str);
        }
        else
        {
            ustr_strcat(in_ustr, in_start_str);
        }
    }
}

ustring*
secdesc_type_to_ustr(secdesc_type in_type)
{
    ustring *ret_val = ustr_new();

    ustring *inside_str = ustr_new();
    add_acl_string(in_type, inside_str,
                   SEC_DESC_DACL_PRESENT,
                   DACL_EXTRA_CODES,
                   dacl_conversions,
                   G_N_ELEMENTS(dacl_conversions),
                   "DACL");
    add_acl_string(in_type, inside_str,
                   SEC_DESC_SACL_PRESENT,
                   SACL_EXTRA_CODES,
                   sacl_conversions,
                   G_N_ELEMENTS(sacl_conversions),
                   "SACL");

    ustr_printfa(ret_val, _("SecDescType(%s)"), ustr_as_utf8(inside_str));
    ustr_free(inside_str);

    return ret_val;
}

secdesc_type
secdesc_type_from_ustr(const ustring *in_ustr)
{
    if (in_ustr == NULL)
    {
        return SECDESC_ERROR_TYPE;
    }

    return 0;
}
