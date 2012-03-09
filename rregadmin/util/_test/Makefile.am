
AM_CPPFLAGS = \
	$(MELUNIT_CXX_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	-I$(top_builddir)/test/lib \
	-I$(top_srcdir)/test/lib \
	-I$(srcdir)

AM_LDFLAGS = $(MELUNIT_CXX_LIBS) # $(EFENCE_LIBS)

LDADD = \
	../librregadmin_util.la

if HAVE_MELUNIT_CXX

check_PROGRAMS = \
	test_binary_search \
	test_conversion_utils \
	test_flag_map \
	test_fs \
	test_intutils \
	test_iconv_helper \
	test_log \
	test_offsets \
	test_offset_list \
	test_options \
	test_path_info \
	test_path_part \
	test_path \
	test_password \
	test_timestamp \
	test_value \
	test_value_type \
	test_ustring \
	test_ustring_list

#	test_uchar_info

TESTS = $(check_PROGRAMS)

test_binary_search_SOURCES = test_binary_search.cpp
test_flag_map_SOURCES = test_flag_map.cpp
test_fs_SOURCES = test_fs.cpp
test_intutils_SOURCES = test_intutils.cpp
test_conversion_utils_SOURCES = test_conversion_utils.cpp
test_iconv_helper_SOURCES = test_iconv_helper.cpp
test_log_SOURCES = test_log.cpp
test_options_SOURCES = test_options.cpp
test_offsets_SOURCES = test_offsets.cpp
test_offset_list_SOURCES = test_offset_list.cpp
test_path_SOURCES = test_path.cpp
test_path_info_SOURCES = test_path_info.cpp
test_path_part_SOURCES = test_path_part.cpp
test_password_SOURCES = test_password.cpp
test_timestamp_SOURCES = test_timestamp.cpp
test_value_SOURCES = test_value.cpp
test_value_type_SOURCES = test_value_type.cpp
#test_uchar_info_SOURCES = test_uchar_info.cpp
test_ustring_SOURCES = test_ustring.cpp
test_ustring_list_SOURCES = test_ustring_list.cpp

clean-local:
	rm -rf test_fs_dir_*

endif
