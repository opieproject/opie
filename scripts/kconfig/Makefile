VERSION=1.2
CC=gcc
CXX=g++
CFLAGS=-O2 -Wall -g -fPIC
CXXFLAGS=$(CFLAGS) -I$(HOSTQTDIR)/include
LDFLAGS=
LXXFLAGS=$(LDFLAGS) -L$(HOSTQTDIR)/lib -Wl,-rpath,$(HOSTQTDIR)/lib
LEX=flex
YACC=bison
YFLAGS=-d -t -v
ifndef HOSTQTDIR
HOSTQTDIR=/usr/share/qt
endif
MOC=$(wildcard $(HOSTQTDIR)/bin/moc)

parse_SRC=zconf.y
conf_SRC=conf.c $(parse_SRC)
mconf_SRC=mconf.c $(parse_SRC)
qconf_SRC=qconf.cc
lkcc_SRC=cml1.y cml1.l help.l cml1.h expr1.c
HDR=expr.h lkc.h lkc_proto.h qconf.h
OTHER=README lkc_spec lkc_overview Makefile.kernel convert-all prepare-all.diff fixup-all.diff \
	kconfig.i extconf.rb example
INST=zconf.y zconf.l confdata.c expr.c symbol.c menu.c conf.c mconf.c qconf.cc kconfig_load.c images.c $(parse_SRC) $(HDR)
INSTGEN=lex.zconf.c zconf.tab.c zconf.tab.h

#DEBUG=1
ifdef DEBUG
CFLAGS+=-DLKC_DIRECT_LINK
qconf_SRC+=$(parse_SRC)
else
qconf_SRC+=kconfig_load.c
endif

SRC=$(conf_SRC) $(mconf_SRC) $(qconf_SRC) $(lkcc_SRC)
CSRC=$(filter %.c, $(SRC))
YSRC=$(filter %.y, $(SRC))
LSRC=$(filter %.l, $(SRC))

parse_OBJ=$(filter %.o, \
		$(patsubst %.c,%.o, \
		$(patsubst %.y,%.tab.o, \
		$(patsubst %.l,lex.%.o, \
			$(parse_SRC)))))
conf_OBJ=$(filter %.o, \
		$(patsubst %.c,%.o, \
		$(patsubst %.y,%.tab.o, \
		$(patsubst %.l,lex.%.o, \
			$(conf_SRC)))))
mconf_OBJ=$(filter %.o, \
		$(patsubst %.c,%.o, \
		$(patsubst %.y,%.tab.o, \
		$(patsubst %.l,lex.%.o, \
			$(mconf_SRC)))))
qconf_OBJ=$(filter %.o, \
		$(patsubst %.c,%.o, \
		$(patsubst %.cc,%.o, \
		$(patsubst %.y,%.tab.o, \
		$(patsubst %.l,lex.%.o, \
			$(qconf_SRC))))))
lkcc_OBJ=$(filter %.o, \
		$(patsubst %.c,%.o, \
		$(patsubst %.y,%.tab.o, \
		$(patsubst %.l,lex.%.o, \
			$(lkcc_SRC)))))
OBJ=$(conf_OBJ) $(mconf_OBJ) $(qconf_OBJ) $(lkcc_OBJ)

ifeq ($(MOC),)
all: lkcc conf mconf
else
all: lkcc conf mconf qconf libkconfig.so
endif

lex.help.c: help.l
lex.help.o: lex.help.c cml1.h expr.h
lex.cml1.c: cml1.l
lex.cml1.o: lex.cml1.c cml1.tab.h cml1.h expr.h
cml1.tab.c: cml1.y
cml1.tab.h: cml1.y
cml1.tab.o: cml1.tab.c cml1.h expr.h
expr1.o: expr1.c expr.h

lkc_deps := lkc.h lkc_proto.h lkc_defs.h expr.h

zconf.tab.c: zconf.y
zconf.tab.h: zconf.y
lex.zconf.c: zconf.l
zconf.tab.o: zconf.tab.c lex.zconf.c confdata.c expr.c symbol.c menu.c $(lkc_deps)
#lex.zconf.o: lex.zconf.c zconf.tab.h $(lkc_deps)
#confdata.o: confdata.c $(lkc_deps)
#expr.o: expr.c $(lkc_deps)
#symbol.o: symbol.c $(lkc_deps)
#menu.o: menu.c $(lkc_deps)
kconfig_load.o: kconfig_load.c $(lkc_deps)
conf.o: conf.c $(lkc_deps)
mconf.o: mconf.c $(lkc_deps)
qconf.moc: qconf.h
qconf.o: qconf.cc qconf.moc images.c $(lkc_deps)

mconf: $(mconf_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

conf: $(conf_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

ifeq ($(MOC),)
qconf:
	@echo Unable to find the QT installation. Please make sure that the
	@echo QT development package is correctly installed and the HOSTQTDIR
	@echo environment variable is set to the correct location.
	@false
else
qconf: $(qconf_OBJ)
	$(CXX) $(LXXFLAGS) $^ -lqt -o $@
endif

lkcc: $(lkcc_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

libkconfig.so: $(parse_OBJ)
	$(CC) -shared $^ -o $@

lkc_defs.h: lkc_proto.h
	sed < $< > $@ 's/P(\([^,]*\),.*/#define \1 (\*\1_p)/'

clean:
	rm -f $(OBJ) lkcc conf qconf mconf *.moc lex.* *.tab.? *.output

tgz:
	mkdir tmp
	mkdir tmp/lkc-$(VERSION)
	cp -ra Makefile $(sort $(SRC) $(HDR) $(OTHER) $(INST)) tmp/lkc-$(VERSION)
	tar -cpvz -C tmp -f lkc-$(VERSION).tar.gz lkc-$(VERSION)
	rm -rf tmp

%.tab.c %.tab.h: %.y
	$(YACC) $(YFLAGS) -b $* -p $* $<

lex.%.c: %.l
	$(LEX) $(LFLAGS) -P$* $<

%.moc: %.h
	$(HOSTQTDIR)/bin/moc -i $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

ifdef KERNELSRC
install: lkcc $(INSTGEN)
	set -x; mkdir $(KERNELSRC)/scripts/kconfig; \
	cp $(sort $(INST)) $(KERNELSRC)/scripts/kconfig; \
	for f in $(INSTGEN); do cp $$f $(KERNELSRC)/scripts/kconfig/$${f}_shipped; done; \
	cp Makefile.kernel $(KERNELSRC)/scripts/kconfig/Makefile; \
	LKCSRC=$$PWD; export LKCSRC; \
	cd $(KERNELSRC); \
	patch -p0 -N < $$LKCSRC/prepare-all.diff; \
	sh $$LKCSRC/convert-all; \
	patch -p0 -N < $$LKCSRC/fixup-all.diff

#	cp Makefile $(KERNELSRC)/scripts/kconfig/Makefile; \

uninstall:
	patch -p0 -N -R -d $(KERNELSRC) < prepare-all.diff; \
	cd $(KERNELSRC); \
	find -name "Kconfig*" | xargs rm; \
	rm -rf scripts/kconfig log.*
else
install:
	@echo "Please use KERNELSRC=<path/to/linux-kernel> to install"
endif

ruby: .ruby libkconfig.so .ruby/kconfig.so

.ruby:
	mkdir .ruby

.ruby/kconfig_wrap.c: kconfig.i kconfig_load.c expr.h lkc_proto.h
	swig -ruby -o $@ $<

.ruby/Makefile: extconf.rb
	cd .ruby; ruby ../extconf.rb

.ruby/kconfig.so: .ruby/kconfig_wrap.c .ruby/Makefile
	make -C .ruby

.PHONY: all tgz clean ruby
