.phony: force
force:

$(configs) :
	$(call makecfg,$@)

$(TOPDIR)/gen.pro : $(TOPDIR)/.config
	echo > $@
ifneq ($(CONFIG_DEBUG),)
	echo CONFIG += debug >> $@
	echo CONFIG -= release >> $@
else
	echo CONFIG -= debug >> $@
	echo CONFIG += release >> $@
endif
ifeq ($(filter 3.%,$(QTE_VERSION)),) # not qt3
	echo CONFIG -= qt3 >> $@
else
	echo CONFIG += qt3 >> $@
endif
ifneq ($(CONFIG_QUICK_LAUNCH),)
	echo contains\( CONFIG, quick-app \) \{ >> $@
	echo CONFIG -= quick-app >> $@
	echo CONFIG += quick-app-lib >> $@
	echo \} >> $@
else
	echo contains\( CONFIG, quick-app \) \{ >> $@
	echo CONFIG -= quick-app >> $@
	echo CONFIG += quick-app-bin >> $@
	echo \} >> $@
endif	

$(TOPDIR)/.depends : $(shell if [ -e $(TOPDIR)/config.in ]\; then echo $(TOPDIR)/config.in\; fi\;) $(TOPDIR)/.config $(TOPDIR)/packages
	@echo Generating dependency information...
# add to subdir-y, and add descend rules
	@cat $(TOPDIR)/packages | grep -v '^#' | \
		awk '{print \
			".PHONY : " $$2 "\n" \
			"subdir-$$(" $$1 ") += " $$2 "\n\n"; \
			print $$2 " : " $$2 "/Makefile\n\t$$(call descend,$$@,$(filter-out $$@,$$(filter-out $$@,$$(MAKECMDGOALS))))\n"; }' > $(TOPDIR)/.depends
	cat $(TOPDIR)/packages | grep -v '^#' | \
		perl -ne '($$cfg, $$dir, $$pro) = $$_ =~ /^(\S+)\s+(\S+)\s+(\S+)/; if ( -e "$$dir/$$pro" ) { print "$$dir/Makefile : $$dir/$$pro \$$(QMAKE) \$$(OPIEDIR)/gen.pro \$$(OPIEDIR)/.config\n\t\$$(call makefilegen,\$$@)\n\n"; }' \
			>> $(TOPDIR)/.depends
# interpackage dependency generation
	@cat $(TOPDIR)/packages | \
		$(TOPDIR)/scripts/deps.pl >> $(TOPDIR)/.depends

$(TOPDIR)/.depends.cfgs:
# config.in interdependencies
	@echo $(configs) | sed -e 's,/config.in,,g' | ( for i in `cat`; do echo $$i; done ) > dirs
	@cat dirs | ( for i in `cat`; do if [ "`cat dirs|grep $$i 2>/dev/null|wc -l`" -ne "1" ]; then deps=`cat dirs|grep $$i| grep -v "^$$i$$"|for i in \`cat|sed -e's,^$(TOPDIR)/,$$(TOPDIR)/,g'\`; do echo $$i/config.in; done`; echo `echo $$i/config.in|sed -e 's,^$(TOPDIR)/,$$(TOPDIR)/,'` : $$deps; fi; done ) >> $@
	@-rm -f dirs

$(QTDIR)/stamp-headers :
	@-rm -f $(QTDIR)/stamp-headers*
	( cd $(QTDIR)/include; \
		$(patsubst %,ln -sf ../src/kernel/%;,qgfx_qws.h qwsmouse_qws.h \
		qcopchannel_qws.h qwindowsystem_qws.h \
		qfontmanager_qws.h qwsdefaultdecoration_qws.h))
	touch $@

$(QTDIR)/stamp-headers-x11 :
	@-rm -f $(QTDIR)/stamp-headers*
	cd $(QTDIR)/include; $(patsubst %,ln -sf $(OPIEDIR)/x11/libqpe-x11/qt/%;,qgfx_qws.h qwsmouse_qws.h qcopchannel_qws.h qwindowsystem_qws.h qfontmanager_qws.h qwsdefaultdecoration_qws.h)
	touch $@

$(OPIEDIR)/stamp-headers :
	@-rm -f $(OPIEDIR)/stamp-headers*
	mkdir -p $(TOPDIR)/include/qpe \
                 $(TOPDIR)/include/qtopia \
		 $(TOPDIR)/include/opie \
		 $(TOPDIR)/include/opie2 \
                 $(TOPDIR)/include/qtopia/private 
	( cd include/qpe &&  rm -f *.h; ln -sf ../../library/*.h .; ln -sf ../../library/backend/*.h .; rm -f *_p.h; )
	( cd include/qtopia && rm -f *.h; ln -sf ../../library/*.h .; )
	( cd include/qtopia/private && rm -f *.h; ln -sf ../../../library/backend/*.h .; )
	( cd include/opie &&  rm -f *.h; ln -sf ../../libopie/*.h .; rm -f *_p.h; )
	( cd include/opie &&  ln -sf ../../libopie/pim/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opiecore/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opiemm/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opiedb/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opienet/*.h .; )
	#( cd include/opie2 && ln -sf ../../libopie2/opiepim/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opieui/*.h .; )
	( cd include/opie; for generatedHeader in `cd ../../libopie; ls *.ui | sed -e "s,\.ui,\.h,g"`; do \
	ln -sf ../../libopie/$$generatedHeader $$generatedHeader; done )
	ln -sf ../../library/custom.h $(TOPDIR)/include/qpe/custom.h
	touch $@
	
$(OPIEDIR)/stamp-headers-x11 :
	@-rm -f $(OPIEDIR)/stamp-headers*
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
	ln -sf ../../library/custom.h $(TOPDIR)/include/qpe/custom.h
	( cd include/qpe; ln -sf ../../x11/libqpe-x11/qpe/*.h .; )
	touch $@
	
$(TOPDIR)/library/custom.h : $(TOPDIR)/.config
	@-rm -f $@
	@$(if $(patsubst "%",%,$(CONFIG_CUSTOMFILE)),\
		ln -sf $(patsubst "%",%,$(CONFIG_CUSTOMFILE)) $@)
	@touch $@

$(TOPDIR)/scripts/lxdialog/lxdialog $(TOPDIR)/scripts/kconfig/mconf $(TOPDIR)/scripts/kconfig/conf $(TOPDIR)/scripts/kconfig/qconf $(TOPDIR)/scripts/kconfig/libkconfig.so $(TOPDIR)/scripts/kconfig/gconf $(TOPDIR)/qmake/qmake:
	@$(call descend,$(shell dirname $@),$(shell basename $@))

menuconfig: $(TOPDIR)/scripts/lxdialog/lxdialog $(TOPDIR)/scripts/kconfig/mconf ./config.in
	$(TOPDIR)/scripts/kconfig/mconf ./config.in
	@touch ./.config.stamp

xconfig: $(TOPDIR)/scripts/kconfig/qconf $(TOPDIR)/scripts/kconfig/libkconfig.so ./config.in
	$(TOPDIR)/scripts/kconfig/qconf ./config.in
	@touch .config.stamp

gconfig: $(TOPDIR)/scripts/kconfig/gconf $(TOPDIR)/scripts/kconfig/libkconfig.so ./config.in
	$(TOPDIR)/scripts/kconfig/gconf ./config.in
	@touch .config.stamp

config: $(TOPDIR)/scripts/kconfig/conf ./config.in
	$(TOPDIR)/scripts/kconfig/conf ./config.in
	@touch .config.stamp
 
oldconfig: $(TOPDIR)/scripts/kconfig/conf ./config.in
	$(TOPDIR)/scripts/kconfig/conf -o ./config.in
	@touch .config.stamp
 
randconfig: $(TOPDIR)/scripts/kconfig/conf ./config.in
	$(TOPDIR)/scripts/kconfig/conf -r ./config.in
	@touch .config.stamp
 
allyesconfig: $(TOPDIR)/scripts/kconfig/conf ./config.in
	$(TOPDIR)/scripts/kconfig/conf -y ./config.in
	@touch .config.stamp
 
allnoconfig: $(TOPDIR)/scripts/kconfig/conf ./config.in
	$(TOPDIR)/scripts/kconfig/conf -n ./config.in
	@touch .config.stamp
 
defconfig: $(TOPDIR)/scripts/kconfig/conf ./config.in
	$(TOPDIR)/scripts/kconfig/conf -d ./config.in
	@touch .config.stamp

$(TOPDIR)/qmake/qmake : $(TOPDIR)/mkspecs/default

$(TOPDIR)/mkspecs/default :
	ln -sf linux-g++ $@

$(TOPDIR)/scripts/subst : force
	@( \
		echo 's,\$$QPE_VERSION,$(QPE_VERSION),g'; \
		echo 's,\$$OPIE_VERSION,$(OPIE_VERSION),g'; \
		echo 's,\$$QTE_VERSION,$(QTE_VERSION),g'; \
		echo 's,\$$QTE_REVISION,$(QTE_REVISION),g'; \
		echo 's,\$$SUB_VERSION,$(SUB_VERSION),g'; \
		echo 's,\$$EXTRAVERSION,$(EXTRAVERSION),g'; \
		echo 's,\$$QTE_BASEVERSION,$(QTE_BASEVERSION),g'; \
	) > $@ || ( rm -f $@; exit 1 )

$(TOPDIR)/scripts/filesubst : force
	@( \
		echo 's,\$$OPIEDIR/root/,/,g'; \
		echo 's,$(OPIEDIR)/root/,/,g'; \
		echo 's,\$$OPIEDIR,$(prefix),g'; \
		echo 's,$(OPIEDIR),$(prefix),g'; \
		echo 's,\$$QTDIR,$(prefix),g'; \
		echo 's,$(QTDIR),$(prefix),g'; \
		echo 's,^\(\./\)*root/,/,g'; \
		echo 's,^\(\./\)*etc/,$(prefix)/etc/,g'; \
		echo 's,^\(\./\)*lib/,$(prefix)/lib/,g'; \
		echo 's,^\(\./\)*bin/,$(prefix)/bin/,g'; \
		echo 's,^\(\./\)*pics/,$(prefix)/pics/,g'; \
		echo 's,^\(\./\)*sounds/,$(prefix)/sounds/,g'; \
		echo 's,^\(\./\)*i18n/,$(prefix)/i18n/,g'; \
		echo 's,^\(\./\)*plugins/,$(prefix)/plugins/,g'; \
		echo 's,^\(\./\)*apps/,$(prefix)/apps/,g'; \
		echo 's,^\(\./\)*share/,$(prefix)/share/,g'; \
		echo 's,^\(\./\)*i18n/,$(prefix)/i18n/,g'; \
		echo 's,^\(\./\)*help/,$(prefix)/help/,g'; \
	) > $@ || ( rm -f $@; exit 1 )

## general rules ##

define descend
	$(MAKE) $(if $(QMAKE),QMAKE=$(QMAKE)) -C $(1) $(2)
endef

define makefilegen
	cd $(if $(1),$(shell dirname $(1))); $(TOPDIR)/qmake/qmake $(3) -o $(if $(1),$(shell basename $(1))) `cat $(OPIEDIR)/packages | grep "	\`echo $(1)|sed -e 's,/Makefile$$,,'\`	" | \
		head -1 | awk '{print $$3}'`
endef

define makecfg
	$(TOPDIR)/scripts/makecfg.pl $1 $(OPIEDIR)
endef
