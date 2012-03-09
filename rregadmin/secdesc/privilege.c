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

#include <rregadmin/config.h>

#include <string.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/secdesc/privilege.h>

struct privilege_mapping
{
    sec_privilege privilege;
    const char *name;
    const char *display_name;
};

static const struct privilege_mapping privilege_names[] =
{
    {
        SEC_PRIV_SECURITY,
        "SeSecurityPrivilege",
        N_("System security")
    },
    {
        SEC_PRIV_BACKUP,
        "SeBackupPrivilege",
        N_("Backup files and directories")
    },
    {
        SEC_PRIV_RESTORE,
        "SeRestorePrivilege",
        N_("Restore files and directories")
    },
    {
        SEC_PRIV_SYSTEMTIME,
        "SeSystemtimePrivilege",
        N_("Set the system clock")
    },
    {
        SEC_PRIV_SHUTDOWN,
        "SeShutdownPrivilege",
        N_("Shutdown the system")
    },
    {
        SEC_PRIV_REMOTE_SHUTDOWN,
        "SeRemoteShutdownPrivilege",
        N_("Shutdown the system remotely")
    },
    {
        SEC_PRIV_TAKE_OWNERSHIP,
        "SeTakeOwnershipPrivilege",
        N_("Take ownership of files and directories")
    },
    {
        SEC_PRIV_DEBUG,
        "SeDebugPrivilege",
        N_("Debug processes")
    },
    {
        SEC_PRIV_SYSTEM_ENVIRONMENT,
        "SeSystemEnvironmentPrivilege",
        N_("Modify system environment")
    },
    {
        SEC_PRIV_SYSTEM_PROFILE,
        "SeSystemProfilePrivilege",
        N_("Profile the system")
    },
    {
        SEC_PRIV_PROFILE_SINGLE_PROCESS,
        "SeProfileSingleProcessPrivilege",
        N_("Profile one process")
    },
    {
        SEC_PRIV_INCREASE_BASE_PRIORITY,
        "SeIncreaseBasePriorityPrivilege",
        N_("Increase base priority")
    },
    {
        SEC_PRIV_LOAD_DRIVER,
        "SeLoadDriverPrivilege",
        N_("Load drivers")
    },
    {
        SEC_PRIV_CREATE_PAGEFILE,
        "SeCreatePagefilePrivilege",
        N_("Create page files")
    },
    {
        SEC_PRIV_INCREASE_QUOTA,
        "SeIncreaseQuotaPrivilege",
        N_("Increase quota")
    },
    {
        SEC_PRIV_CHANGE_NOTIFY,
        "SeChangeNotifyPrivilege",
        N_("Register for change notify")
    },
    {
        SEC_PRIV_UNDOCK,
        "SeUndockPrivilege",
        N_("Undock devices")
    },
    {
        SEC_PRIV_MANAGE_VOLUME,
        "SeManageVolumePrivilege",
        N_("Manage system volumes")
    },
    {
        SEC_PRIV_IMPERSONATE,
        "SeImpersonatePrivilege",
        N_("Impersonate users")
    },
    {
        SEC_PRIV_CREATE_GLOBAL,
        "SeCreateGlobalPrivilege",
        N_("Create global")
    },
    {
        SEC_PRIV_ENABLE_DELEGATION,
        "SeEnableDelegationPrivilege",
        N_("Enable Delegation")
    },
    {
        SEC_PRIV_INTERACTIVE_LOGON,
        "SeInteractiveLogonRight",
        N_("Interactive logon")
    },
    {
        SEC_PRIV_NETWORK_LOGON,
        "SeNetworkLogonRight",
        N_("Network logon")
    },
    {
        SEC_PRIV_REMOTE_INTERACTIVE_LOGON,
        "SeRemoteInteractiveLogonRight",
        N_("Remote Interactive logon")
    },
};


const char*
sec_privilege_name(sec_privilege privilege)
{
    int i;
    for (i = 0; i < G_N_ELEMENTS(privilege_names); i++)
    {
        if (privilege_names[i].privilege == privilege)
        {
            return privilege_names[i].name;
        }
    }
    return NULL;
}

const char*
sec_privilege_display_name(sec_privilege privilege)
{
    int i;
    if (privilege < 1 || privilege > 64)
    {
        return NULL;
    }
    for ( i = 0; i < G_N_ELEMENTS(privilege_names); i++)
    {
        if (privilege_names[i].privilege == privilege)
        {
            return _(privilege_names[i].display_name);
        }
    }
    return NULL;
}

sec_privilege
sec_privilege_id(const char *name)
{
    int i;
    for (i = 0; i < G_N_ELEMENTS(privilege_names); i++)
    {
        if (strcasecmp(privilege_names[i].name, name) == 0)
        {
            return privilege_names[i].privilege;
        }
    }
    return -1;
}

guint64
sec_privilege_mask(sec_privilege privilege)
{
    guint64 mask = 1;

    if (privilege < 1 || privilege > 64)
    {
        return 0;
    }

    mask <<= (privilege-1);
    return mask;
}
