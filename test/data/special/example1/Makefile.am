
EXTRA_DIST = hives README

link_from_location = $(top_srcdir)/test/data/registries/example2/system32/config

registry-tree:
	@$(mkinstalldirs) system32/config
	@( \
	  cd system32/config ; \
	  ln -sf ../../$(link_from_location)/system System ; \
	  ln -sf ../../$(link_from_location)/default default ; \
	  ln -sf ../../$(link_from_location)/SAM SAM ; \
	  ln -sf ../../$(link_from_location)/SECURITY SECURITY ; \
	  ln -sf ../../$(link_from_location)/software Software ; \
	  ln -sf ../../$(link_from_location)/userdiff userdiff ; \
	)

all-local: registry-tree

clean-local:
	rm -rf system32
