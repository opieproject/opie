## targets ##

$(configs) :
	$(call makecfg,$@)

$(TOPDIR)/.depends : $(TOPDIR)/config.in
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
	# generation of config.in files, and config.in interdependencies
	find ./ -name config.in | ( for cfg in `cat`; do dir=`dirname $cfg`; name=`basename $dir`; if [ ! -e $dir/$name.pro ]; then echo $dir; fi; done; ) > dirs
	cat dirs | for i in `cat`; do numlines="`cat dirs|grep $i 2>/dev/null|wc -l`"; if [ "$numlines" -ne "1" ]; then deps=`cat dirs|grep $i| grep -v "^$i$"|for i in \`cat|sed -e's,^./,$(TOPDIR)/,g'\`; do echo $i/config.in; done`; echo `echo $i|sed -e 's,^./,$(TOPDIR)/,'` : $deps; fi; done;

$(TOPDIR)/qmake/qmake :
	$(call descend,$(TOPDIR)/qmake)

$(TOPDIR)/scripts/lxdialog/lxdialog $(TOPDIR)/scripts/kconfig/conf scripts/kconfig/conf $(TOPDIR)/scripts/kconfig/mconf scripts/kconfig/mconf $(TOPDIR)/scripts/kconfig/qconf scripts/kconfig/qconf :
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
