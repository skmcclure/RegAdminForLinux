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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <ctype.h>

#include <fstream>
#include <string>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>

#include <glib/gstrfuncs.h>
#include <glib/gfileutils.h>
#include <glib/gmem.h>

#include <melunit/melunit-cxx.h>

#include <rregadmin/registry/registry.h>
#include <rregadmin/registry/registry_key.h>
#include <rregadmin/registry/hive_info.h>
#include <rregadmin/registry/init.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/util/value.h>
#include <rregadmin/util/ustring.h>

#include "good_stuff.h"
#include "FileInfo.hpp"
#include "example_registry.h"
#include "log_level_setter.hpp"

namespace
{
    using namespace RRegAdmin::Test;
    namespace ba = boost::algorithm;

    class test_registry2 : public Melunit::Test
    {
    private:

        bool test_load_unload1(int ehindex)
        {
            Registry *reg1 = registry_new();
            assert_not_null(reg1);

            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            for (int i = 0; i < hlist.size(); i++)
            {
                assert_equal(0, registry_number_of_hives(reg1));
                reg_id id1 = registry_load_hive(reg1, hlist[i].c_str());
                if (id1 == -1)
                    continue;

                assert_equal(1, registry_number_of_hives(reg1));

                assert_true(registry_unload(reg1, id1));
                assert_equal(0, registry_number_of_hives(reg1));
            }

            assert_true(registry_free(reg1));

            return true;
        }

        bool test_load_unload2(int ehindex)
        {
            Registry *reg1 = registry_new();
            assert_not_null(reg1);

            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            for (int i = 0; i < hlist.size(); i++)
            {
                assert_equal(0, registry_number_of_hives(reg1));
                reg_id id1 = registry_load_hive(reg1, hlist[i].c_str());
                if (id1 == -1)
                    continue;
                assert_compare(std::not_equal_to, -1, id1);
                assert_equal(1, registry_number_of_hives(reg1));

                assert_true(registry_unload(reg1, id1));
                assert_equal(0, registry_number_of_hives(reg1));
            }

            assert_true(registry_free(reg1));

            return true;
        }

        bool test_load_all(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            assert_true(registry_free(reg1));

            return true;
        }

        bool test_dirty1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            assert_false(registry_set_dirty(reg1, TRUE));

            assert_false(registry_unload_all(reg1));
            assert_true(registry_flush_all(reg1));

            assert_true(registry_unload_all(reg1));
            assert_equal_msg(0, registry_number_of_hives(reg1),
                             er1.get_name());
            assert_true(registry_free(reg1));

            return true;
        }

        bool test_dirty2(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            assert_false(registry_set_dirty(reg1, TRUE));

            assert_false(registry_unload_all(reg1));
            assert_true(registry_set_dirty(reg1, FALSE));

            assert_true(registry_unload_all(reg1));
            assert_equal_msg(0, registry_number_of_hives(reg1),
                             er1.get_name());
            assert_true(registry_free(reg1));

            return true;
        }

        bool test_dirty3(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            assert_false(registry_set_dirty(reg1, TRUE));

            assert_false(registry_unload_all(reg1));
            assert_true(registry_unload_all_force(reg1));

            assert_equal_msg(0, registry_number_of_hives(reg1),
                             er1.get_name());
            assert_true(registry_free(reg1));

            return true;
        }

        bool test_add_key1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            assert_false(registry_is_dirty(reg1));

            KeyCell *kc1 = registry_path_add_key(
                reg1, "\\HKEY_LOCAL_MACHINE\\system\\foobar");
            assert_not_null_msg(kc1, er1.get_name());

            ustring *kc1_name = key_cell_get_name(kc1);
            assert_not_null(kc1_name);
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(kc1_name)));
            ustr_free(kc1_name);

            assert_true(registry_is_dirty(reg1));

            assert_false(registry_unload_all(reg1));

            assert_true(registry_unload_all_force(reg1));
            assert_equal_msg(0, registry_number_of_hives(reg1),
                             er1.get_name());
            assert_true(registry_free(reg1));

            return true;
        }

        bool test_add_key2(int ehindex)
        {
            Registry *reg1 = registry_new();
            assert_not_null(reg1);

            assert_equal(0, registry_number_of_hives(reg1));

            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            int count_modifier = 1;

            for (int i = 0; i < hlist.size(); i++)
            {
                assert_equal(i, registry_number_of_hives(reg1));
                reg_id id1 = registry_load_hive(reg1, hlist[i].c_str());
                if (id1 == -1)
                {
                    count_modifier--;
                    continue;
                }

                assert_equal(i + count_modifier,
                             registry_number_of_hives(reg1));
            }

            assert_false(registry_is_dirty(reg1));

            const char *key_path = "\\HKEY_LOCAL_MACHINE\\system\\foobar";
            KeyCell *kc1 = registry_path_add_key(reg1, key_path);
            do_if_fail(
                assert_not_null(kc1),
                do
                {
                    std::string ls_cmd = "ls -lR ";
                    ls_cmd += er1.get_dir();
                    system(ls_cmd.c_str());
                    ustring *tmp_out = ustr_new();
                    registry_get_xml_output(reg1, tmp_out);
                    std::cerr << ustr_as_utf8(tmp_out) << std::endl;
                    ustr_free(tmp_out);

                } while(0));

            ustring *kc1_name = key_cell_get_name(kc1);
            assert_not_null(kc1_name);
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(kc1_name)));
            ustr_free(kc1_name);

            assert_true(registry_is_dirty(reg1));

            assert_false(registry_unload_all(reg1));

            assert_true(registry_flush_all(reg1));
            assert_true(registry_unload_all(reg1));
            assert_equal(0, registry_number_of_hives(reg1));
            assert_true(registry_free(reg1));

            reg1 = registry_new();
            assert_not_null(reg1);

            assert_equal(0, registry_number_of_hives(reg1));
            count_modifier = 1;

            for (int i = 0; i < hlist.size(); i++)
            {
                assert_equal(i, registry_number_of_hives(reg1));
                reg_id id1 = registry_load_hive(reg1, hlist[i].c_str());
                if (id1 == -1)
                {
                    count_modifier--;
                    continue;
                }

                assert_equal(i + count_modifier,
                             registry_number_of_hives(reg1));
            }

            assert_false(registry_is_dirty(reg1));

            assert_true(registry_path_is_key(reg1, key_path));
            KeyCell *kc2 = registry_path_get_key(reg1, key_path);
            assert_not_null(kc2);

            assert_true(registry_unload_all(reg1));
            assert_equal(0, registry_number_of_hives(reg1));
            assert_true(registry_free(reg1));

            return true;
        }

        bool test_add_value1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            assert_false(registry_is_dirty(reg1));

            // Value *val1 = value_create_string("blah blah", 10, USTR_TYPE_UTF8);
            Value *val1 = value_create_dword(44);

            ValueKeyCell *vkc1 = registry_path_add_value(
                reg1, "\\HKEY_LOCAL_MACHINE\\system", "foobar", val1);
            assert_not_null(vkc1);

            ustring *kc1_name = value_key_cell_get_name(vkc1);
            assert_not_null(kc1_name);
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(kc1_name)));
            ustr_free(kc1_name);

            assert_true(registry_is_dirty(reg1));

            assert_false(registry_unload_all(reg1));

            assert_true(registry_unload_all_force(reg1));
            assert_equal_msg(0, registry_number_of_hives(reg1),
                             er1.get_name());
            assert_true(registry_free(reg1));

            return true;
        }

        bool test_add_string_value1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            assert_false(registry_is_dirty(reg1));

            Value *val1 = value_create_string("blah blah", 10, USTR_TYPE_UTF8);

            ValueKeyCell *vkc1 = registry_path_add_value(
                reg1, "\\HKEY_LOCAL_MACHINE\\system", "foobar", val1);
            assert_not_null(vkc1);

            ustring *kc1_name = value_key_cell_get_name(vkc1);
            assert_not_null(kc1_name);
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(kc1_name)));
            ustr_free(kc1_name);

            assert_true(registry_is_dirty(reg1));

            assert_false(registry_unload_all(reg1));

            assert_true(registry_unload_all_force(reg1));
            assert_equal_msg(0, registry_number_of_hives(reg1),
                             er1.get_name());
            assert_true(registry_free(reg1));

            return true;
        }

        bool test_add_value2(int ehindex)
        {
            Registry *reg1 = registry_new();
            assert_not_null(reg1);

            assert_equal(0, registry_number_of_hives(reg1));

            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            std::vector<std::string> hlist = er1.get_hive_list();
            int count_modifier = 1;

            for (int i = 0; i < hlist.size(); i++)
            {
                assert_equal(i, registry_number_of_hives(reg1));
                reg_id id1 = registry_load_hive(reg1, hlist[i].c_str());
                if (id1 == -1)
                {
                    count_modifier--;
                    continue;
                }

                assert_equal(i + count_modifier,
                             registry_number_of_hives(reg1));
            }

            assert_false(registry_is_dirty(reg1));

            const char *key_path = "\\HKEY_LOCAL_MACHINE\\system";
            const char *val_name = "foobar";
            // Value *val1 = value_create_string("blah blah", 9, USTR_TYPE_UTF8);
            Value *val1 = value_create_dword(44);

            ValueKeyCell *vkc1 = registry_path_add_value(reg1, key_path,
                                                         val_name, val1);
            assert_not_null(vkc1);

            ustring *kc1_name = value_key_cell_get_name(vkc1);
            assert_not_null(kc1_name);
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(kc1_name)));
            ustr_free(kc1_name);

            assert_true(registry_is_dirty(reg1));

            assert_false(registry_unload_all(reg1));

            assert_true(registry_flush_all(reg1));
            assert_true(registry_unload_all(reg1));
            assert_equal(0, registry_number_of_hives(reg1));
            assert_true(registry_free(reg1));

            reg1 = registry_new();
            assert_not_null(reg1);

            assert_equal(0, registry_number_of_hives(reg1));
            count_modifier = 1;

            for (int i = 0; i < hlist.size(); i++)
            {
                assert_equal(i, registry_number_of_hives(reg1));
                reg_id id1 = registry_load_hive(reg1, hlist[i].c_str());
                if (id1 == -1)
                {
                    count_modifier--;
                    continue;
                }

                assert_equal(i + count_modifier,
                             registry_number_of_hives(reg1));
            }

            assert_false(registry_is_dirty(reg1));

            assert_true(registry_path_is_value(reg1, key_path, val_name));
            ValueKeyCell *vkc2 = registry_path_get_value(reg1, key_path,
                                                         val_name);
            assert_not_null(vkc2);
            Value *val2 = value_key_cell_get_val(vkc2);
            assert_equal(REG_DWORD, value_get_type(val2));

            assert_true(registry_unload_all(reg1));
            assert_equal(0, registry_number_of_hives(reg1));
            assert_true(registry_free(reg1));

            return true;
        }

        bool test_add_key_recursive1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            assert_false(registry_is_dirty(reg1));

            KeyCell *kc1 = registry_path_add_key(
                reg1, "\\HKEY_LOCAL_MACHINE\\system\\foobar\\blah\\baz");
            assert_not_null(kc1);

            ustring *kc1_name = key_cell_get_name(kc1);
            assert_not_null(kc1_name);
            assert_equal(std::string("baz"),
                         std::string(ustr_as_utf8(kc1_name)));
            ustr_free(kc1_name);

            KeyCell *kcp1 = key_cell_get_parent(kc1);
            assert_not_null(kcp1);
            ustring *kcp1_name = key_cell_get_name(kcp1);
            assert_not_null(kcp1_name);
            assert_equal(std::string("blah"),
                         std::string(ustr_as_utf8(kcp1_name)));
            ustr_free(kcp1_name);

            KeyCell *kcp2 = key_cell_get_parent(kcp1);
            assert_not_null(kcp2);
            ustring *kcp2_name = key_cell_get_name(kcp2);
            assert_not_null(kcp2_name);
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(kcp2_name)));
            ustr_free(kcp2_name);

            assert_true(registry_is_dirty(reg1));

            assert_false(registry_unload_all(reg1));

            assert_true(registry_unload_all_force(reg1));
            assert_equal_msg(0, registry_number_of_hives(reg1),
                             er1.get_name());
            assert_true(registry_free(reg1));

            return true;
        }

        bool test_delete_key1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            assert_false(registry_is_dirty(reg1));

            const ef_line *line = RRegadmin::Test::get_good_key();

            if (line == 0)
                return false;

            assert_true(registry_path_is_key(reg1, line->path));
            assert_true(registry_path_delete_key(reg1, line->path));
            assert_false(registry_path_is_key(reg1, line->path));

            assert_true(registry_flush_all(reg1));
            assert_true(registry_free(reg1));

            return true;
        }

        bool test_delete_value1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            assert_false(registry_is_dirty(reg1));

            const ef_line *line = RRegadmin::Test::get_good_value();

            if (line == 0)
                return false;

            assert_true(registry_path_is_value(reg1, line->path,
                                               line->name));
            assert_true_msg(registry_path_delete_value(reg1, line->path,
                                                       line->name),
                            line->path);
            assert_false(registry_path_is_value(reg1, line->path,
                                                line->name));

            assert_true(registry_flush_all(reg1));
            assert_true(registry_free(reg1));

            return true;
        }

        bool test_backup_option_nochange1(int ehindex)
        {
            Registry *reg1 = registry_new();
            assert_not_null(reg1);

            assert_equal(0, registry_number_of_hives(reg1));

            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            std::vector<FileInfo> infos;
            int count_modifier = 1;

            for (int i = 0; i < hlist.size(); i++)
            {
                assert_equal(i, registry_number_of_hives(reg1));
                FileInfo tmp_fi(hlist[i]);
                infos.push_back(tmp_fi);
                reg_id id1 = registry_load_hive(reg1, hlist[i].c_str());
                if (id1 == -1)
                {
                    count_modifier--;
                    continue;
                }

                assert_equal(i + count_modifier,
                             registry_number_of_hives(reg1));
            }

            assert_true(registry_unload_all(reg1));
            assert_equal(0, registry_number_of_hives(reg1));
            assert_true(registry_free(reg1));

            for (int i = 0; i < hlist.size(); i++)
            {
                FileInfo test_fi(hlist[i]);
                assert_equal(infos[i], test_fi);

                std::string bkup_name = hlist[i];
                bkup_name += ".hbk";

                assert_false(g_file_test(bkup_name.c_str(),
                                         G_FILE_TEST_EXISTS));
            }

            return true;
        }

        bool test_options1(void)
        {
            Registry *reg1 = registry_new();
            assert_not_null(reg1);

            assert_false(registry_has_option(reg1, REG_OPT_BACKUP));

            assert_true(registry_set_option(reg1, REG_OPT_BACKUP));
            assert_true(registry_has_option(reg1, REG_OPT_BACKUP));

            assert_true(registry_set_option(reg1, REG_OPT_BACKUP));
            assert_true(registry_has_option(reg1, REG_OPT_BACKUP));

            assert_true(registry_unset_option(reg1, REG_OPT_BACKUP));
            assert_false(registry_has_option(reg1, REG_OPT_BACKUP));

            assert_true(registry_unset_option(reg1, REG_OPT_BACKUP));
            assert_false(registry_has_option(reg1, REG_OPT_BACKUP));

            assert_true(registry_free(reg1));
            return true;
        }

        bool test_backup_option_nochange2(int ehindex)
        {
            Registry *reg1 = registry_new();
            assert_not_null(reg1);

            assert_equal(0, registry_number_of_hives(reg1));

            assert_true(registry_set_option(reg1, REG_OPT_BACKUP));
            assert_true(registry_has_option(reg1, REG_OPT_BACKUP));

            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            std::vector<FileInfo> infos;
            int count_modifier = 1;

            for (int i = 0; i < hlist.size(); i++)
            {
                assert_equal(i, registry_number_of_hives(reg1));
                FileInfo tmp_fi(hlist[i]);
                infos.push_back(tmp_fi);
                reg_id id1 = registry_load_hive(reg1, hlist[i].c_str());
                if (id1 == -1)
                {
                    count_modifier--;
                    continue;
                }

                assert_compare(std::not_equal_to, -1, id1);
                assert_equal(i + count_modifier,
                             registry_number_of_hives(reg1));
            }

            assert_true(registry_unload_all(reg1));
            assert_equal(0, registry_number_of_hives(reg1));
            assert_true(registry_free(reg1));

            for (int i = 0; i < hlist.size(); i++)
            {
                FileInfo test_fi(hlist[i]);
                assert_equal(infos[i], test_fi);

                std::string bkup_name = hlist[i];
                bkup_name += ".hbk";

                assert_false(g_file_test(bkup_name.c_str(),
                                         G_FILE_TEST_EXISTS));
            }

            return true;
        }

        bool test_backup_option1(int ehindex)
        {
            Registry *reg1 = registry_new();
            assert_not_null(reg1);

            assert_equal(0, registry_number_of_hives(reg1));

            assert_true(registry_set_option(reg1, REG_OPT_BACKUP));
            assert_true(registry_has_option(reg1, REG_OPT_BACKUP));

            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            std::vector<FileInfo> infos;
            int count_modifier = 1;

            for (int i = 0; i < hlist.size(); i++)
            {
                assert_equal(i, registry_number_of_hives(reg1));
                FileInfo tmp_fi(hlist[i]);
                infos.push_back(tmp_fi);
                reg_id id1 = registry_load_hive(reg1, hlist[i].c_str());
                if (id1 == -1)
                {
                    count_modifier--;
                    continue;
                }

                assert_equal(i + count_modifier,
                             registry_number_of_hives(reg1));
            }

            KeyCell *kc1 = registry_path_add_key(
                reg1, "\\HKEY_LOCAL_MACHINE\\system\\foobar");
            assert_not_null(kc1);

            assert_true(registry_is_dirty(reg1));
            assert_true(registry_flush_all(reg1));

            assert_true(registry_unload_all(reg1));
            assert_equal(0, registry_number_of_hives(reg1));
            assert_true(registry_free(reg1));

            for (int i = 0; i < hlist.size(); i++)
            {
                if (ba::ends_with(hlist[i], "system"))
                {
                    // These turned into warning until we have time to fix
                    // case insensitive file checks.
                    std::string bkup_name = hlist[i];
                    bkup_name += ".hbk";
                    assert_warn(
                        assert_true(g_file_test(bkup_name.c_str(),
                                                G_FILE_TEST_EXISTS)));
                    FileInfo test_fi_bkup(bkup_name);

                    assert_warn(assert_equal(infos[i], test_fi_bkup));

                    FileInfo test_fi(hlist[i]);
                    assert_warn(
                        assert_compare(std::not_equal_to, infos[i], test_fi));
                }
                else
                {
                    FileInfo test_fi(hlist[i]);
                    assert_equal(infos[i], test_fi);

                    std::string bkup_name = hlist[i];
                    bkup_name += ".hbk";

                    assert_false(g_file_test(bkup_name.c_str(),
                                             G_FILE_TEST_EXISTS));
                }
            }

            return true;
        }

        bool test_backup_option_noset1(int ehindex)
        {
            Registry *reg1 = registry_new();
            assert_not_null(reg1);

            assert_equal(0, registry_number_of_hives(reg1));

            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            std::vector<FileInfo> infos;
            int count_modifier = 1;

            for (int i = 0; i < hlist.size(); i++)
            {
                assert_equal(i, registry_number_of_hives(reg1));
                FileInfo tmp_fi(hlist[i]);
                infos.push_back(tmp_fi);
                reg_id id1 = registry_load_hive(reg1, hlist[i].c_str());
                if (id1 == -1)
                {
                    count_modifier--;
                    continue;
                }

                assert_equal(i + count_modifier,
                             registry_number_of_hives(reg1));
            }

            KeyCell *kc1 = registry_path_add_key(
                reg1, "\\HKEY_LOCAL_MACHINE\\system\\foobar");
            assert_not_null(kc1);

            assert_true(registry_is_dirty(reg1));
            assert_true(registry_flush_all(reg1));

            assert_true(registry_unload_all(reg1));
            assert_equal(0, registry_number_of_hives(reg1));
            assert_true(registry_free(reg1));

            for (int i = 0; i < hlist.size(); i++)
            {
                FileInfo test_fi(hlist[i]);
                if (ba::ends_with(hlist[i], "system"))
                {
                    assert_compare(std::not_equal_to, infos[i], test_fi);
                }
                else
                {
                    assert_equal(infos[i], test_fi);
                }

                std::string bkup_name = hlist[i];
                bkup_name += ".hbk";

                assert_false(g_file_test(bkup_name.c_str(),
                                         G_FILE_TEST_EXISTS));
            }

            return true;
        }

        static const int min_step = 10;

        void cikey_helper(Registry *in_reg, ef_line *in_line, int index = 0)
        {
            if (false)
            {
                std::cerr << "Checking for key: " << in_line->path << std::endl;
            }

            int step;

            if (index > strlen(in_line->path))
            {
                index = strlen(in_line->path);
                step = 1;
            }
            else
            {
                step = (strlen(in_line->path) - index) / 5;
                step = (step >= min_step ? step : min_step);
            }

            assert_true_msg(registry_path_is_key(in_reg, in_line->path),
                            in_line->path);
            in_line->path[index] = toupper(in_line->path[index]);
            assert_true_msg(registry_path_is_key(in_reg, in_line->path),
                            in_line->path);

            if (index < strlen(in_line->path))
            {
                cikey_helper(in_reg, in_line, index + step);
                in_line->path[index] = tolower(in_line->path[index]);
                cikey_helper(in_reg, in_line, index + step);
            }
        }

        bool test_caseinsensitive_key1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            for (int j = 0; j < 1; j++)
            {
                const ef_line *tmp_line = RRegadmin::Test::get_good_key();
                if (tmp_line == 0)
                    break;
                ef_line line;
                line.path = g_strdup(tmp_line->path);
                line.name = NULL;
                cikey_helper(reg1, &line);
            }

            assert_true(registry_free(reg1));

            return true;
        }

        void civalue_helper(Registry *in_reg, ef_line *line, int index = 0)
        {
            if (false)
            {
                std::cerr << "Checking for value:" << line->path
                          << " " << line->name << std::endl;
            }

            int step;

            if (index > strlen(line->path))
            {
                index = strlen(line->path);
                step = 1;
            }
            else
            {
                step = (strlen(line->path) - index) / 5;
                step = (step >= min_step ? step : min_step);
            }

            assert_true_msg(registry_path_is_value(in_reg, line->path,
                                                   line->name),
                            line->path);
            line->path[index] = toupper(line->path[index]);
            assert_true_msg(registry_path_is_value(in_reg, line->path,
                                                   line->name),
                            line->path);

            if (index < strlen(line->path))
            {
                civalue_helper(in_reg, line, index + step);
                line->path[index] = tolower(line->path[index]);
                civalue_helper(in_reg, line, index + step);
            }
        }

        bool test_caseinsensitive_value1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            for (int j = 0; j < 1; j++)
            {
                const ef_line *tmp_line = RRegadmin::Test::get_good_value();
                if (tmp_line == 0)
                    break;
                ef_line line;
                line.path = g_strdup(tmp_line->path);
                line.name = g_strdup(tmp_line->name);
                civalue_helper(reg1, &line);
            }

            assert_true(registry_free(reg1));

            return true;
        }

        bool test_good_keys1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            const std::vector<ef_line*> &keys =
                RRegadmin::Test::get_good_keys();

            for (int j = 0; j < keys.size(); j += (rand() % 50) + 50)
            {
                assert_true_msg(registry_path_is_key(reg1, keys[j]->path),
                                keys[j]->path);
            }

            assert_true(registry_free(reg1));

            return true;
        }

        bool test_good_values1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            const std::vector<ef_line*> &values =
                RRegadmin::Test::get_good_values();

            for (int j = 0; j < values.size(); j += (rand() % 50) + 50)
            {
                assert_true_msg(
                    registry_path_is_value(reg1, values[j]->path,
                                           values[j]->name),
                    values[j]->path);
            }

            assert_true(registry_free(reg1));

            return true;
        }

        bool test_contains_hivetype1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            HiveType good_types[] = {
                HTYPE_SYSTEM,
                HTYPE_SECURITY,
                HTYPE_SOFTWARE,
                HTYPE_SAM,
                HTYPE_USERDIFF,
                HTYPE_DEFAULT
            };

            for (int i = 0; i < G_N_ELEMENTS(good_types); i++)
            {
                assert_true(registry_contains_hivetype(reg1, good_types[i]));
            }

            assert_true(registry_free(reg1));

            return true;
        }

    public:

        test_registry2(): Melunit::Test("test_registry2")
        {
            rra_registry_init();
            rra_registry_init_from_env();

#define REGISTER(name) register_test(#name, &test_registry2:: name)
#define REG_BOOSTB(name, ehindex) \
            register_test(#name, \
                          boost::bind(&test_registry2:: name, this, ehindex))

            REGISTER(test_options1);

            for (int i = 0; i < RRegAdmin::Test::ExampleReg::size(); i++)
            {
                REG_BOOSTB(test_load_all, i);
                REG_BOOSTB(test_load_unload2, i);
                REG_BOOSTB(test_load_unload1, i);
                REG_BOOSTB(test_dirty1, i);
                REG_BOOSTB(test_dirty2, i);
                REG_BOOSTB(test_dirty3, i);
                REG_BOOSTB(test_add_key1, i);
                REG_BOOSTB(test_add_key2, i);
                REG_BOOSTB(test_add_value1, i);
                REG_BOOSTB(test_add_value2, i);
                REG_BOOSTB(test_add_string_value1, i);
                REG_BOOSTB(test_add_key_recursive1, i);
                REG_BOOSTB(test_delete_key1, i);
                REG_BOOSTB(test_delete_value1, i);
                REG_BOOSTB(test_backup_option_nochange1, i);
                REG_BOOSTB(test_backup_option_nochange2, i);
                REG_BOOSTB(test_backup_option1, i);
                REG_BOOSTB(test_backup_option_noset1, i);
                REG_BOOSTB(test_caseinsensitive_key1, i);
                REG_BOOSTB(test_caseinsensitive_value1, i);
                REG_BOOSTB(test_good_keys1, i);
                REG_BOOSTB(test_good_values1, i);
                REG_BOOSTB(test_contains_hivetype1, i);
            }

            Melunit::Suite::instance().register_test(this);

#undef REGISTER
#undef REG_BOOSTB
        }

    };

    test_registry2 t1_;
}
