
EXTRA_DIST = \
	beforeAfter.diff \
	system.hiv.win2003.after \
	system.hiv.win2003.before \
	system.reg.win2003.after \
	system.reg.win2003.before

PATH_FILES = system.hiv.win2003.after.paths system.hiv.win2003.before.paths
all-local: $(PATH_FILES)

system.hiv.win2003.%.paths: system.reg.win2003.% Makefile.am
	grep '^\[' $< | sed 's/^\[\(.*\)\].*/\\\1/' | grep -i '^\\hkey_local_machine\\system' > $@

DISTCLEANFILES = $(PATH_FILES)
