## targets ##

$(configs) :
	$(call makecfg,$@)

$(TOPDIR)/.depends : $(shell if [ -e $(TOPDIR)/config.in ]\; then echo $(TOPDIR)/config.in\; fi\;) $(TOPDIR)/.config $(TOPDIR)/packages
	@echo Generating dependency information...
# add to subdir-y, and add descend rules
	@cat $(TOPDIR)/packages | \
		awk '/^#/ { next }; {print \
			".PHONY : " $$2 "\n" \
			"subdir-$$(" $$1 ") += " $$2 "\n\n" \
			$$2 "/Makefile : " $$2 "/" $$3 " $$(TOPDIR)/qmake/qmake\n\t" \
			"$$(call makefilegen,$$@)\n\n" \
			$$2 " : " $$2 "/Makefile\n\t$$(call descend,$$@)\n"}'\
			> $(TOPDIR)/.depends
# interpackage dependency generation
	@cat $(TOPDIR)/packages | \
		$(TOPDIR)/scripts/deps.pl >> $(TOPDIR)/.depends

$(TOPDIR)/.depends.cfgs:
# generation of config.in files, and config.in interdependencies
	@find $(TOPDIR)/ -name config.in | ( for cfg in `cat`; do dir=`dirname $$cfg`; name=`basename $$dir`; if [ ! -e $$dir/$$name.pro ]; then echo $$dir; fi; done; ) > dirs
	@echo "configs += `echo \`cat dirs | sed -e's,^$(TOPDIR)/,$$(TOPDIR)/,g' -e's,$$,/config.in,g'\``" >> $@
	@cat dirs | ( for i in `cat`; do if [ "`cat dirs|grep $$i 2>/dev/null|wc -l`" -ne "1" ]; then deps=`cat dirs|grep $$i| grep -v "^$$i$$"|for i in \`cat|sed -e's,^$(TOPDIR)/,$$(TOPDIR)/,g'\`; do echo $$i/config.in; done`; echo `echo $$i/config.in|sed -e 's,^$(TOPDIR)/,$$(TOPDIR)/,'` : $$deps; fi; done ) >> $@
	@-rm -f dirs

$(TOPDIR)/scripts/lxdialog/lxdialog $(TOPDIR)/scripts/kconfig/conf scripts/kconfig/conf $(TOPDIR)/scripts/kconfig/mconf scripts/kconfig/mconf $(TOPDIR)/scripts/kconfig/qconf scripts/kconfig/qconf $(TOPDIR)/qmake/qmake :
	$(call descend,$(shell dirname $@),$(shell basename $@))

## general rules ##

define descend
	$(MAKE) -C $(1) $(2)
endef

define makefilegen
	cd $(shell dirname $(1)); $(TOPDIR)/qmake/qmake $(3) -o $(shell basename $(1)) `echo $(1)|sed -e 's,/Makefile$$,,g' -e 's,.*/,,g'`.pro
endef

define makecfg
	$(TOPDIR)/scripts/makecfg.pl $1
endef
