#!/usr/bin/make -f

export TOPDIR:=$(OPIEDIR)

include $(TOPDIR)/Vars.make
ifneq ($(wildcard $(TOPDIR)/Vars.local),)
include $(TOPDIR)/Vars.local
endif

noconfig_targets := xconfig menuconfig config oldconfig randconfig \
		    defconfig allyesconfig allnoconfig allmodconfig \
		    clean-configs $(TOPDIR)/scripts/subst $(TOPDIR)/scripts/filesubst \
		    ipks

configs += $(TOPDIR)/core/applets/config.in $(TOPDIR)/core/apps/config.in $(TOPDIR)/core/multimedia/config.in $(TOPDIR)/core/pim/config.in $(TOPDIR)/core/pim/today/plugins/config.in $(TOPDIR)/core/settings/config.in $(TOPDIR)/development/config.in $(TOPDIR)/inputmethods/config.in  $(TOPDIR)/libopie/pim/config.in $(TOPDIR)/noncore/applets/config.in $(TOPDIR)/noncore/apps/opie-console/test/config.in $(TOPDIR)/noncore/apps/config.in $(TOPDIR)/noncore/comm/config.in $(TOPDIR)/noncore/decorations/config.in $(TOPDIR)/noncore/games/config.in $(TOPDIR)/noncore/graphics/config.in $(TOPDIR)/noncore/multimedia/config.in $(TOPDIR)/noncore/net/config.in $(TOPDIR)/noncore/net/opietooth/config.in $(TOPDIR)/noncore/settings/config.in $(TOPDIR)/noncore/styles/config.in $(TOPDIR)/noncore/tools/config.in $(TOPDIR)/noncore/todayplugins/config.in $(TOPDIR)/examples/config.in

# $(TOPDIR)/.config depends on .depends.cfgs, as it depends on $(configs)
# in order to have a full set of config.in files.
# .depends depends on $(TOPDIR)/.config
# everything else depends on .depends, to ensure the dependencies are
# intact.
#
# NOTE: The order in which things happen in this makefile is
# 	-critical-. Do not rearrange this!
 
all : $(TOPDIR)/.config

#
# The IPK creation is a very slow process. If you want to only create some
# IPKs, e.g. the ones in library, then do
#	make ipks IPK_START=library
# and then only the *.control files in this directory will be processed
ipks: $(OPIEDIR)/scripts/subst $(OPIEDIR)/scripts/filesubst FORCE $(TOPDIR)/.config
	@find $(OPIEDIR)/$(IPK_START) -type f -name \*.control | ( for ctrl in `cat`; do \
		prerm=`echo $${ctrl/.control/.prerm}`; \
		preinst=`echo $${ctrl/.control/.preinst}`; \
		postrm=`echo $${ctrl/.control/.postrm}`; \
		postinst=`echo $${ctrl/.control/.postinst}`; \
		echo "Building ipk of $$ctrl"; \
		cd $(OPIEDIR); $(OPIEDIR)/scripts/mkipkg --subst=$(OPIEDIR)/scripts/subst --filesubst=$(OPIEDIR)/scripts/filesubst --control=$$ctrl --prerm=$$prerm --preinst=$$preinst --postrm=$$postrm --postinst=$$postinst --strip=$(STRIP) $(OPIEDIR); \
		done )

ipks-mt: $(OPIEDIR)/scripts/subst $(OPIEDIR)/scripts/filesubst FORCE $(TOPDIR)/.config
	@> $(OPIEDIR)/AllThreadedPackages
	@find $(OPIEDIR)/ -type f -name \*.control | grep -v -- "-mt" | while read ctrl ; do \
		grep "Package[ ]*:" $${ctrl} | sed "s+Package[ ]*:[ ]*++"; \
		done | sort | uniq >> $(OPIEDIR)/AllThreadedPackages
	@find $(OPIEDIR)/ -type f -name \*.control | while read ctrl ; do \
		echo "Converting $$ctrl to -mt package"; \
		nctrl=`$(OPIEDIR)/scripts/tothreaded $$ctrl $(OPIEDIR)/AllThreadedPackages`; \
		echo "Building ipk of $$ctrl"; \
		[ -n $$nctrl ] && cd $(OPIEDIR) && $(OPIEDIR)/scripts/mkipkg --subst=$(OPIEDIR)/scripts/subst --filesubst=$(OPIEDIR)/scripts/filesubst --control=$$nctrl --prerm=$${nctrl/\.control$$/.prerm/} --preinst=$${nctrl/\.control$$/.preinst/} --postrm=$${nctrl/\.control$$/.postrm/} --postinst=$${nctrl/\.control$$/.postinst/} --strip=$(STRIP) $(OPIEDIR); \
		done
	@rm -f $(OPIEDIR)/AllThreadedPackages

FORCE:

$(TOPDIR)/.config : $(TOPDIR)/.depends.cfgs

all menuconfig xconfig oldconfig config randconfig allyesconfig allnoconfig defconfig : $(TOPDIR)/.depends.cfgs

clean-configs :
	@echo "Wiping generated config.in files..."
	@-rm -f $(configs)

ifneq ($(wildcard $(TOPDIR)/.depends.cfgs),)
    include $(TOPDIR)/.depends.cfgs
endif

all menuconfig xconfig oldconfig config randconfig allyesconfig allnoconfig defconfig : $(configs)

$(TOPDIR)/.config: $(TOPDIR)/.depends.cfgs $(configs)
	$(call descend,scripts/kconfig,conf)
	@if [ ! -e $@ ]; then \
		cp $(TOPDIR)/def-configs/opie $@; \
	fi
	@$(MAKE) -C scripts/kconfig conf
	./scripts/kconfig/conf -s ./config.in

export

ifeq ($(filter $(noconfig_targets),$(MAKECMDGOALS)),)

export include-config := 1

-include $(TOPDIR)/.config
-include $(TOPDIR)/.depends
endif

-include $(TOPDIR)/.config.cmd

SUBDIRS = $(subdir-y)

all clean install ipk: $(SUBDIRS)

lupdate lrelease:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

opie-lupdate opie-lrelease messages:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done;

# from kde 
qtmessages:
	cd $(QTDIR)/src ; \
	sed -e "s,#define,," xml/qxml.cpp > qxml_clean.cpp ;\
	find . -name "*.cpp" | grep -v moc_ > list ;\
	for file in qfiledialog qcolordialog qprintdialog \
		qurloperator qftp qhttp qlocal qerrormessage; do \
		grep -v $$file list > list.new && mv list.new list ;\
	done ;\
	xgettext -C -ktr -kQT_TRANSLATE_NOOP -n `cat list` -o $(OPIEDIR)/qt-messages.pot 

$(subdir-y) : $(if $(CONFIG_LIBQPE),$(QTDIR)/stamp-headers $(OPIEDIR)/stamp-headers) \
	$(if $(CONFIG_LIBQPE-X11),$(QTDIR)/stamp-headers-x11 $(OPIEDIR)/stamp-headers-x11 ) \
	$(TOPDIR)/library/custom.h

clean : $(TOPDIR)/.config
	make -C bin clean
	make -C lib clean
	make -C plugins clean

apidox : 
	doc/generate_apidox 

mrproper : clean-configs
	find . -name ".moc"|xargs rm -rf
	find . -name ".obj"|xargs rm -rf
	find . -name "*.pro"|xargs touch

include $(TOPDIR)/Rules.make
