## targets ##

$(TOPDIR)/config.in :
	@-rm -f $@
	@for cfg in `find $(TOPDIR) -name config.in`; do \
		echo 'include $$cfg' >> $(TOPDIR)/config.in; \
	done;

$(TOPDIR)/.depends :
	@cat $(TOPDIR)/packages | \
		awk '{print \
			".PHONY : " $$2 "\n" \
			"subdir-$$(" $$1 ") += " $$2 "\n\n" \
			$$2 "/Makefile : " $$2 "/" $$3 " $$(TOPDIR)/qmake/qmake\n\t" \
			"$$(call makefilegen,$$@)\n\n" \
			$$2 " : " $$2 "/Makefile\n\t$$(call descend,$$@)\n"}'\
			> $(TOPDIR)/.depends

$(TOPDIR)/qmake/qmake :
	$(call descend,$(TOPDIR)/qmake)

$(TOPDIR)/scripts/lxdialog/lxdialog :
	$(call descend,$(TOPDIR)/scripts/lxdialog)

$(TOPDIR)/scripts/kconfig/conf scripts/kconfig/conf:
	$(call descend,$(TOPDIR)/scripts/kconfig,conf)

$(TOPDIR)/scripts/kconfig/mconf scripts/kconfig/mconf:
	$(call descend,$(TOPDIR)/scripts/kconfig,mconf)

$(TOPDIR)/scripts/kconfig/qconf scripts/kconfijg/qconf:
	$(call descend,$(TOPDIR)/scripts/kconfig,qconf)

## general rules ##

define descend
	$(MAKE) -C $(1) $(2)
endef

define makefilegen
	cd $(shell dirname $(1)); $(TOPDIR)/qmake/qmake $(3) -o $(shell basename $(1)) `echo $(1)|sed -e 's,/Makefile$$,,g' -e 's,.*/,,g'`.pro
endef
