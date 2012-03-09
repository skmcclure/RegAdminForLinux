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

#include <stdio.h>

#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/security_descriptor_cell.h>
#include <rregadmin/secdesc/security_descriptor.h>
#include <rregadmin/secdesc/aclist.h>
#include <rregadmin/secdesc/acentry.h>

static void
print_acl(const char *in_tag, ACList *in_acl)
{
    if (aclist_get_acentry_count(in_acl) == 0)
    {
        return;
    }

    acentry_access_mask entry_mask = 0;
    acentry_flags entry_flags = 0;

    printf("%s Rev: %d : Types(", in_tag, aclist_get_rev(in_acl));

    int i;
    for (i = 0; i < aclist_get_acentry_count(in_acl); i++)
    {
        const ACEntry *entry = aclist_get_acentry(in_acl, i);
        printf("%d", acentry_get_type(entry));

        entry_mask |= acentry_get_access_mask(entry);
        entry_flags |= acentry_get_flags(entry);
    }

    printf("), CombinedMask: 0x%08x, CombinedFlags: 0x%04x\n",
           entry_mask, entry_flags);
}

static void
print_acls(const SecurityDescriptor *in_sd)
{
    printf("SecDescType: 0x%08x\n", secdesc_get_type(in_sd));
    ACList *dacl = secdesc_get_dacl((SecurityDescriptor*)in_sd);
    if (dacl != NULL)
    {
        print_acl("DACL", dacl);
    }
    ACList *sacl = secdesc_get_sacl((SecurityDescriptor*)in_sd);
    if (sacl != NULL)
    {
        print_acl("SACL", sacl);
    }
}

int
main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf (stderr, "Usage: %s <hive file>\n", argv[0]);
        exit(1);
    }

    Hive *hv = hive_read(argv[1]);

    SecurityDescriptorCell *sdc = hive_get_first_security_descriptor_cell(hv);

    while (sdc != NULL)
    {
        print_acls(security_descriptor_cell_get_secdesc(sdc));
        sdc = security_descriptor_cell_get_next(sdc);
    }

    hive_free(hv);

    return 0;
}
