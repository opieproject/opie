#!/usr/bin/make -f

export TOPDIR:=$(shell pwd)

include $(TOPDIR)/Vars.make

noconfig_targets := xconfig menuconfig config oldconfig randconfig \
		    defconfig allyesconfig allnoconfig allmodconfig \
		    clean-configs $(TOPDIR)/scripts/subst $(TOPDIR)/scripts/filesubst \
		    ipks

configs += $(TOPDIR)/core/applets/config.in $(TOPDIR)/core/apps/config.in $(TOPDIR)/core/multimedia/config.in $(TOPDIR)/core/pim/config.in $(TOPDIR)/core/pim/today/plugins/config.in $(TOPDIR)/core/settings/config.in $(TOPDIR)/development/config.in $(TOPDIR)/inputmethods/config.in $(TOPDIR)/libopie/ofileselector/config.in $(TOPDIR)/libopie/pim/config.in $(TOPDIR)/libsql/config.in $(TOPDIR)/noncore/applets/config.in $(TOPDIR)/noncore/apps/opie-console/test/config.in $(TOPDIR)/noncore/apps/config.in $(TOPDIR)/noncore/comm/config.in $(TOPDIR)/noncore/decorations/config.in $(TOPDIR)/noncore/games/config.in $(TOPDIR)/noncore/graphics/config.in $(TOPDIR)/noncore/multimedia/config.in $(TOPDIR)/noncore/net/config.in $(TOPDIR)/noncore/net/opietooth/config.in $(TOPDIR)/noncore/settings/config.in $(TOPDIR)/noncore/styles/config.in $(TOPDIR)/noncore/tools/calc2/config.in $(TOPDIR)/noncore/tools/config.in $(TOPDIR)/noncore/todayplugins/config.in

# $(TOPDIR)/.config depends on .depends.cfgs, as it depends on $(configs)
# in order to have a full set of config.in files.
# .depends depends on $(TOPDIR)/.config
# everything else depends on .depends, to ensure the dependencies are
# intact.
#
# NOTE: The order in which things happen in this makefile is
# 	-critical-. Do not rearrange this!
 
all : $(TOPDIR)/.config

ipks: $(OPIEDIR)/scripts/subst $(OPIEDIR)/scripts/filesubst FORCE $(TOPDIR)/.config
	@find $(OPIEDIR)/ -type f -name \*.control | ( for ctrl in `cat`; do \
		prerm=`echo $$ctrl|sed -e 's,\.control$$,.prerm,'`; \
		preinst=`echo $$ctrl|sed -e 's,\.control$$,.preinst,'`; \
		postrm=`echo $$ctrl|sed -e 's,\.control$$,.postrm,'`; \
		postinst=`echo $$ctrl|sed -e 's,\.control$$,.postinst,'`; \
		echo "Building ipk of $$ctrl"; \
		cd $(OPIEDIR); $(OPIEDIR)/scripts/mkipkg --subst=$(OPIEDIR)/scripts/subst --filesubst=$(OPIEDIR)/scripts/filesubst --control=$$ctrl --prerm=$$prerm --preinst=$$preinst --postrm=$$postrm --postinst=$$postinst --strip=$(STRIP) $(OPIEDIR); \
		done )

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
	fi;
	@$(MAKE) -C scripts/kconfig conf;
	./scripts/kconfig/conf -s ./config.in

# config rules must have the $(configs) var defined
# at the time that they run. we must ensure that .depends.cfgs
# is built and included by the time we reach this point.

xconfig :
	$(call descend,scripts/kconfig,qconf)
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(TOPDIR)/scripts/kconfig \
		./scripts/kconfig/qconf ./config.in

menuconfig : scripts/lxdialog/lxdialog
	$(call descend,scripts/kconfig,mconf)
	./scripts/kconfig/mconf ./config.in

config :
	$(call descend,scripts/kconfig,conf)
	./scripts/kconfig/conf ./config.in
 
oldconfig :
	$(call descend,scripts/kconfig,conf)
	./scripts/kconfig/conf -o ./config.in
 
randconfig :
	$(call descend,scripts/kconfig,conf)
	./scripts/kconfig/conf -r ./config.in
 
allyesconfig :
	$(call descend,scripts/kconfig,conf)
	./scripts/kconfig/conf -y ./config.in
 
allnoconfig :
	$(call descend,scripts/kconfig,conf)
	./scripts/kconfig/conf -n ./config.in
 
defconfig :
	$(call descend,scripts/kconfig,conf)
	./scripts/kconfig/conf -d ./config.in


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
	@for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done;

opie-lupdate opie-lrelease:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done;

$(subdir-y) : $(if $(CONFIG_LIBQPE),$(QTDIR)/stamp-headers $(OPIEDIR)/stamp-headers) \
	$(if $(CONFIG_LIBQPE-X11),$(QTDIR)/stamp-headers-x11 $(OPIEDIR)/stamp-headers-x11 ) \
	$(TOPDIR)/library/custom.h

clean : $(TOPDIR)/.config

apidox : doc/generate_apidox 

include $(TOPDIR)/Rules.make
