## targets ##

$(configs) :
	$(call makecfg,$@)

$(TOPDIR)/.depends : $(shell if [ -e $(TOPDIR)/config.in ]\; then echo $(TOPDIR)/config.in\; fi\;) $(TOPDIR)/.config $(TOPDIR)/packages
	@echo Generating dependency information...
# add to subdir-y, and add descend rules
	cat $(TOPDIR)/packages | grep -v '^#' | \
		awk '{print \
			".PHONY : " $$2 "\n" \
			"subdir-$$(" $$1 ") += " $$2 "\n\n"; \
			print $$2 " : " $$2 "/Makefile\n\t$$(call descend,$$@,$(filter-out $$@,$$(MAKECMDGOALS)))\n"; }' > $(TOPDIR)/.depends
	cat $(TOPDIR)/packages | grep -v '^#' | \
		perl -ne '($$cfg, $$dir, $$pro) = $$_ =~ /^(\S+)\s+(\S+)\s+(\S+)/; if ( -e "$$dir/$$pro" ) { print "$$dir/Makefile : $$dir/$$pro \$$(TOPDIR)/qmake/qmake\n\t\$$(call makefilegen,\$$@)\n\n"; }' \
			>> $(TOPDIR)/.depends
# interpackage dependency generation
	cat $(TOPDIR)/packages | \
		$(TOPDIR)/scripts/deps.pl >> $(TOPDIR)/.depends

$(TOPDIR)/.depends.cfgs:
# config.in interdependencies
	@echo $(configs) | sed -e 's,/config.in,,g' | ( for i in `cat`; do echo $$i; done ) > dirs
	@cat dirs | ( for i in `cat`; do if [ "`cat dirs|grep $$i 2>/dev/null|wc -l`" -ne "1" ]; then deps=`cat dirs|grep $$i| grep -v "^$$i$$"|for i in \`cat|sed -e's,^$(TOPDIR)/,$$(TOPDIR)/,g'\`; do echo $$i/config.in; done`; echo `echo $$i/config.in|sed -e 's,^$(TOPDIR)/,$$(TOPDIR)/,'` : $$deps; fi; done ) >> $@
	@-rm -f dirs

$(TOPDIR)/stamp-headers :
	mkdir -p $(TOPDIR)/include/qpe $(TOPDIR)/include/qtopia \
		$(TOPDIR)/include/opie $(TOPDIR)/include/qtopia/private
	( cd include/qpe &&  rm -f *.h; ln -sf ../../library/*.h .; ln -sf ../../library/backend/*.h .; rm -f *_p.h; )
	( cd include/qtopia && rm -f *.h; ln -sf ../../library/*.h .; )
	( cd include/qtopia/private && rm -f *.h; ln -sf ../../../library/backend/*.h .; )
	( cd include/opie &&  rm -f *.h; ln -sf ../../libopie/*.h .; rm -f *_p.h; )
	( cd include/opie &&  ln -sf ../../libsql/*.h .; )
	( cd include/opie &&  ln -sf ../../libopie/pim/*.h .; )
	( cd include/opie; for generatedHeader in `cd ../../libopie; ls *.ui | sed -e "s,\.ui,\.h,g"`; do \
	ln -sf ../../libopie/$$generatedHeader $$generatedHeader; done )
	touch $@
	
$(TOPDIR)/library/custom.h : $(TOPDIR)/.config
	@-rm -f $@
	$(if $(patsubst "%",%,$(CONFIG_CUSTOMFILE)),\
		ln -sf $(patsubst "%",%,$(CONFIG_CUSTOMFILE)) $@,\
		touch $@\
	)

$(TOPDIR)/scripts/lxdialog/lxdialog $(TOPDIR)/scripts/kconfig/conf scripts/kconfig/conf $(TOPDIR)/scripts/kconfig/mconf scripts/kconfig/mconf $(TOPDIR)/scripts/kconfig/qconf scripts/kconfig/qconf $(TOPDIR)/qmake/qmake :
	$(call descend,$(shell dirname $@),$(shell basename $@))

$(TOPDIR)/qmake/qmake : $(TOPDIR)/mkspecs/default

$(TOPDIR)/mkspecs/default :
	ln -sf linux-g++ $@

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
