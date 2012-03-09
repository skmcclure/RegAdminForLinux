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

#include <rregadmin/util/binary_search.h>
#include <rregadmin/util/init.h>

namespace
{
    gconstpointer get_num_array_entry(gconstpointer data, gint entry)
    {
        gint *num_data = (gint*)data;
        return (gconstpointer)num_data[entry];
    }

    gint compare_numbers(gconstpointer a, gconstpointer b)
    {
        return GPOINTER_TO_INT(b) - GPOINTER_TO_INT(a);
    }

    class test_binary_search : public Melunit::Test
    {
    private:

        bool test_simple1(void)
        {
            gint data1[] = { 5, 6, 10, 12 };

            assert_equal(2, rra_binary_search(0, 3, (gconstpointer)10,
                                             data1, get_num_array_entry,
                                             compare_numbers, 0));
            assert_equal(0, rra_binary_search(0, 3, (gconstpointer)5,
                                             data1, get_num_array_entry,
                                             compare_numbers, 0));
            assert_equal(3, rra_binary_search(0, 3, (gconstpointer)12,
                                             data1, get_num_array_entry,
                                             compare_numbers, 0));
            assert_equal(1, rra_binary_search(0, 3, (gconstpointer)6,
                                             data1, get_num_array_entry,
                                             compare_numbers, 0));
            assert_equal(-1, rra_binary_search(0, 3, (gconstpointer)3,
                                              data1, get_num_array_entry,
                                              compare_numbers, 0));
            assert_equal(-1, rra_binary_search(0, 3, (gconstpointer)40,
                                              data1, get_num_array_entry,
                                              compare_numbers, 0));
            assert_equal(-1, rra_binary_search(0, 3, (gconstpointer)0,
                                              data1, get_num_array_entry,
                                              compare_numbers, 0));

            return true;
        }

        bool test_simple2(void)
        {
            gint data1[] = { 5, 6, 10, 12 };
            int one_before;

            assert_equal(2, rra_binary_search(0, 3, (gconstpointer)10,
                                             data1, get_num_array_entry,
                                             compare_numbers, &one_before));
            assert_equal(1, one_before);

            assert_equal(0, rra_binary_search(0, 3, (gconstpointer)5,
                                             data1, get_num_array_entry,
                                             compare_numbers, &one_before));
            assert_equal(-1, one_before);

            assert_equal(3, rra_binary_search(0, 3, (gconstpointer)12,
                                             data1, get_num_array_entry,
                                             compare_numbers, &one_before));
            assert_equal(2, one_before);

            assert_equal(1, rra_binary_search(0, 3, (gconstpointer)6,
                                             data1, get_num_array_entry,
                                             compare_numbers, &one_before));
            assert_equal(0, one_before);

            assert_equal(-1, rra_binary_search(0, 3, (gconstpointer)3,
                                              data1, get_num_array_entry,
                                              compare_numbers, &one_before));
            assert_equal(-1, one_before);

            assert_equal(-1, rra_binary_search(0, 3, (gconstpointer)40,
                                              data1, get_num_array_entry,
                                              compare_numbers, &one_before));
            assert_equal(3, one_before);

            assert_equal(-1, rra_binary_search(0, 3, (gconstpointer)0,
                                              data1, get_num_array_entry,
                                              compare_numbers, &one_before));
            assert_equal(-1, one_before);

            assert_equal(-1, rra_binary_search(0, 3, (gconstpointer)9,
                                              data1, get_num_array_entry,
                                              compare_numbers, &one_before));
            assert_equal(1, one_before);


            return true;
        }

    public:

        test_binary_search(): Melunit::Test("test_binary_search")
        {
#define REGISTER(name) register_test(#name, &test_binary_search:: name)

            REGISTER(test_simple1);
            REGISTER(test_simple2);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_binary_search t1_;

    struct bs_gpa_data
    {
        int start_val;
        int len;

        bs_gpa_data(int in_sv, int in_len)
            : start_val(in_sv),
              len(in_len)
        {
        }
    };

    gint compare_bs_gpa_data(gconstpointer a, gconstpointer b)
    {
        struct bs_gpa_data *a_data = (struct bs_gpa_data*)a;
        int b_data = GPOINTER_TO_INT(b);

//         printf("Comparing %d to bs_gpa_data(%d, %d)\n",
//                b_data, a_data->start_val, a_data->len);

        if (b_data < a_data->start_val)
        {
            return -1;
        }

        if (b_data >= a_data->start_val
            && b_data < (a_data->start_val + a_data->len))
        {
            return 0;
        }

        return 1;
    }

    GPtrArray* build_bgd_gptrarray(const gint* in_int_array, int in_arr_len)
    {
        GPtrArray *ret_val = g_ptr_array_new();

        for (int i = 0; i < in_arr_len - 1; i++)
        {
            bs_gpa_data *elem = new bs_gpa_data(
                in_int_array[i],
                (in_int_array[i + 1] - in_int_array[i]));
            g_ptr_array_add(ret_val, elem);
        }

        return ret_val;
    }

    void destroy_bgd_gptrarray(GPtrArray *in_arr)
    {
        while (in_arr->len > 0)
        {
            bs_gpa_data *elem =
                (bs_gpa_data*)g_ptr_array_remove_index_fast(in_arr, 0);
            delete elem;
        }
        g_ptr_array_free(in_arr, TRUE);
    }

    class test_binary_search_gptrarray : public Melunit::Test
    {
    private:

        bool test_data_gen1(void)
        {
            gint data1[] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
            GPtrArray *arr = build_bgd_gptrarray(data1, G_N_ELEMENTS(data1));

            assert_equal(10, arr->len);
            int val = 0;
            for (int i = 0; i < arr->len; i++)
            {
                bs_gpa_data *d = (bs_gpa_data*)g_ptr_array_index(arr, i);
                assert_equal(val, d->start_val);
                assert_equal(10, d->len);
                val += 10;
            }

            destroy_bgd_gptrarray(arr);
            return true;
        }

        bool test_simple1(void)
        {
            gint data1[] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
            GPtrArray *arr = build_bgd_gptrarray(data1, G_N_ELEMENTS(data1));

            gint arr_item = rra_binary_search_gptrarray(arr,
                                                        GINT_TO_POINTER(15),
                                                        compare_bs_gpa_data,
                                                        0);

            assert_equal(1, arr_item);

            bs_gpa_data *d = (bs_gpa_data*)g_ptr_array_index(arr, arr_item);
            assert_equal(10, d->start_val);

            destroy_bgd_gptrarray(arr);
            return true;
        }

        bool test_simple2(void)
        {
            gint data1[] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
            GPtrArray *arr = build_bgd_gptrarray(data1, G_N_ELEMENTS(data1));

            for (int i = 0; i < data1[G_N_ELEMENTS(data1) - 1]; i++)
            {
                gint one_before;
                gint arr_item = rra_binary_search_gptrarray(
                    arr, GINT_TO_POINTER(i), compare_bs_gpa_data,
                    &one_before);

                gint expected_item = i / 10;

                assert_equal(expected_item, arr_item);

                bs_gpa_data *d =
                    (bs_gpa_data*)g_ptr_array_index(arr, arr_item);
                assert_equal(expected_item * 10, d->start_val);

                assert_equal(expected_item - 1, one_before);
            }

            destroy_bgd_gptrarray(arr);
            return true;
        }

    public:

        test_binary_search_gptrarray()
            : Melunit::Test("test_binary_search_gptrarray")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, \
                                     &test_binary_search_gptrarray:: name)

            REGISTER(test_data_gen1);
            REGISTER(test_simple1);
            REGISTER(test_simple2);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_binary_search_gptrarray t2_;
}
