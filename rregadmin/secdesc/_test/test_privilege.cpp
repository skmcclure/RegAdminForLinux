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


#include <melunit/melunit-cxx.h>

#include <rregadmin/secdesc/privilege.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_privilege : public Melunit::Test
    {
    private:

        bool test_to_name(void)
        {
            assert_equal(
                std::string("SeSecurityPrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_SECURITY)));
            assert_equal(
                std::string("SeBackupPrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_BACKUP)));
            assert_equal(
                std::string("SeRestorePrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_RESTORE)));
            assert_equal(
                std::string("SeSystemtimePrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_SYSTEMTIME)));
            assert_equal(
                std::string("SeShutdownPrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_SHUTDOWN)));
            assert_equal(
                std::string("SeRemoteShutdownPrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_REMOTE_SHUTDOWN)));
            assert_equal(
                std::string("SeTakeOwnershipPrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_TAKE_OWNERSHIP)));
            assert_equal(
                std::string("SeDebugPrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_DEBUG)));
            assert_equal(
                std::string("SeSystemEnvironmentPrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_SYSTEM_ENVIRONMENT)));
            assert_equal(
                std::string("SeSystemProfilePrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_SYSTEM_PROFILE)));
            assert_equal(
                std::string("SeProfileSingleProcessPrivilege"),
                std::string(sec_privilege_name(
                                SEC_PRIV_PROFILE_SINGLE_PROCESS)));
            assert_equal(
                std::string("SeIncreaseBasePriorityPrivilege"),
                std::string(sec_privilege_name(
                                SEC_PRIV_INCREASE_BASE_PRIORITY)));
            assert_equal(
                std::string("SeLoadDriverPrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_LOAD_DRIVER)));
            assert_equal(
                std::string("SeCreatePagefilePrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_CREATE_PAGEFILE)));
            assert_equal(
                std::string("SeIncreaseQuotaPrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_INCREASE_QUOTA)));
            assert_equal(
                std::string("SeChangeNotifyPrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_CHANGE_NOTIFY)));
            assert_equal(
                std::string("SeUndockPrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_UNDOCK)));
            assert_equal(
                std::string("SeManageVolumePrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_MANAGE_VOLUME)));
            assert_equal(
                std::string("SeImpersonatePrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_IMPERSONATE)));
            assert_equal(
                std::string("SeCreateGlobalPrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_CREATE_GLOBAL)));
            assert_equal(
                std::string("SeEnableDelegationPrivilege"),
                std::string(sec_privilege_name(SEC_PRIV_ENABLE_DELEGATION)));
            assert_equal(
                std::string("SeInteractiveLogonRight"),
                std::string(sec_privilege_name(SEC_PRIV_INTERACTIVE_LOGON)));
            assert_equal(
                std::string("SeNetworkLogonRight"),
                std::string(sec_privilege_name(SEC_PRIV_NETWORK_LOGON)));
            assert_equal(
                std::string("SeRemoteInteractiveLogonRight"),
                std::string(sec_privilege_name(
                                SEC_PRIV_REMOTE_INTERACTIVE_LOGON)));
            return true;
        }

        bool test_to_id(void)
        {
            assert_equal(SEC_PRIV_SECURITY,
                         sec_privilege_id("SeSecurityPrivilege"));
            assert_equal(SEC_PRIV_BACKUP,
                         sec_privilege_id("SeBackupPrivilege"));
            assert_equal(SEC_PRIV_RESTORE,
                         sec_privilege_id("SeRestorePrivilege"));
            assert_equal(SEC_PRIV_SYSTEMTIME,
                         sec_privilege_id("SeSystemtimePrivilege"));
            assert_equal(SEC_PRIV_SHUTDOWN,
                         sec_privilege_id("SeShutdownPrivilege"));
            assert_equal(SEC_PRIV_REMOTE_SHUTDOWN,
                         sec_privilege_id("SeRemoteShutdownPrivilege"));
            assert_equal(SEC_PRIV_TAKE_OWNERSHIP,
                         sec_privilege_id("SeTakeOwnershipPrivilege"));
            assert_equal(SEC_PRIV_DEBUG,
                         sec_privilege_id("SeDebugPrivilege"));
            assert_equal(SEC_PRIV_SYSTEM_ENVIRONMENT,
                         sec_privilege_id("SeSystemEnvironmentPrivilege"));
            assert_equal(SEC_PRIV_SYSTEM_PROFILE,
                         sec_privilege_id("SeSystemProfilePrivilege"));
            assert_equal(SEC_PRIV_PROFILE_SINGLE_PROCESS,
                         sec_privilege_id("SeProfileSingleProcessPrivilege"));
            assert_equal(SEC_PRIV_INCREASE_BASE_PRIORITY,
                         sec_privilege_id("SeIncreaseBasePriorityPrivilege"));
            assert_equal(SEC_PRIV_LOAD_DRIVER,
                         sec_privilege_id("SeLoadDriverPrivilege"));
            assert_equal(SEC_PRIV_CREATE_PAGEFILE,
                         sec_privilege_id("SeCreatePagefilePrivilege"));
            assert_equal(SEC_PRIV_INCREASE_QUOTA,
                         sec_privilege_id("SeIncreaseQuotaPrivilege"));
            assert_equal(SEC_PRIV_CHANGE_NOTIFY,
                         sec_privilege_id("SeChangeNotifyPrivilege"));
            assert_equal(SEC_PRIV_UNDOCK,
                         sec_privilege_id("SeUndockPrivilege"));
            assert_equal(SEC_PRIV_MANAGE_VOLUME,
                         sec_privilege_id("SeManageVolumePrivilege"));
            assert_equal(SEC_PRIV_IMPERSONATE,
                         sec_privilege_id("SeImpersonatePrivilege"));
            assert_equal(SEC_PRIV_CREATE_GLOBAL,
                         sec_privilege_id("SeCreateGlobalPrivilege"));
            assert_equal(SEC_PRIV_ENABLE_DELEGATION,
                         sec_privilege_id("SeEnableDelegationPrivilege"));
            assert_equal(SEC_PRIV_INTERACTIVE_LOGON,
                         sec_privilege_id("SeInteractiveLogonRight"));
            assert_equal(SEC_PRIV_NETWORK_LOGON,
                         sec_privilege_id("SeNetworkLogonRight"));
            assert_equal(SEC_PRIV_REMOTE_INTERACTIVE_LOGON,
                         sec_privilege_id("SeRemoteInteractiveLogonRight"));
            return true;
        }

    public:

        test_privilege(): Melunit::Test("test_privilege")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_privilege:: name)

            REGISTER(test_to_name);
            REGISTER(test_to_id);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_privilege t1_;
}
