VERSION=1.3
CC=gcc
CXX=g++
CFLAGS=-O0 -Wall -g -fPIC
CXXFLAGS=$(CFLAGS) -I$(HOSTQTDIR)/include
LDFLAGS=
LXXFLAGS=$(LDFLAGS) -L$(HOSTQTDIR)/lib -Wl,-rpath,$(HOSTQTDIR)/lib
LEX=flex
LFLAGS=-L
YACC=bison
YFLAGS=-l
#YFLAGS=-d -t -v -l
ifndef HOSTQTDIR
HOSTQTDIR=/usr/share/qt
endif
MOC=$(wildcard $(HOSTQTDIR)/bin/moc)

parse_SRC=zconf.y
conf_SRC=conf.c $(parse_SRC)
mconf_SRC=mconf.c $(parse_SRC)
qconf_SRC=qconf.cc
HDR=expr.h lkc.h lkc_proto.h qconf.h
OTHER=README lkc-language.txt Makefile.kernel \
	kconfig.i extconf.rb example
INST=zconf.y zconf.l confdata.c expr.c symbol.c menu.c \
	conf.c mconf.c qconf.cc kconfig_load.c images.c $(HDR)
INSTGEN=lex.zconf.c zconf.tab.c

#DEBUG=1
ifdef DEBUG
CFLAGS+=-DLKC_DIRECT_LINK
qconf_SRC+=$(parse_SRC)
else
qconf_SRC+=kconfig_load.c
endif

SRC=$(conf_SRC) $(mconf_SRC) $(qconf_SRC)
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
OBJ=$(conf_OBJ) $(mconf_OBJ) $(qconf_OBJ)

ifeq ($(MOC),)
all: conf mconf
else
all: conf mconf qconf libkconfig.so
endif

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

libkconfig.so: $(parse_OBJ)
	$(CC) -shared $^ -o $@

lkc_defs.h: lkc_proto.h
	sed < $< > $@ 's/P(\([^,]*\),.*/#define \1 (\*\1_p)/'

clean:
	rm -f $(OBJ) conf qconf mconf *.moc lex.* *.tab.? *.output
	rm -rf .ruby .python

tgz:
	mkdir tmp
	mkdir tmp/lkc-$(VERSION)
	cp -ra Makefile $(sort $(SRC) $(HDR) $(OTHER) $(INST)) tmp/lkc-$(VERSION)
	tar -cpvz -C tmp --exclude CVS -f lkc-$(VERSION).tar.gz lkc-$(VERSION)
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
install: $(INSTGEN)
	set -x; cp $(sort $(INST)) $(KERNELSRC)/scripts/kconfig; \
	for f in $(INSTGEN); do cp $$f $(KERNELSRC)/scripts/kconfig/$${f}_shipped; done; \
	cp Makefile.kernel $(KERNELSRC)/scripts/kconfig/Makefile

diff: $(INSTGEN)
	for f in $(sort $(INST)); do diff -u $(KERNELSRC)/scripts/kconfig/$$f $$f; done; \
	for f in $(INSTGEN); do diff -u $(KERNELSRC)/scripts/kconfig/$${f}_shipped $$f; done; \
	diff -u $(KERNELSRC)/scripts/kconfig/Makefile Makefile.kernel
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


PYTHON_INCLUDE=$(shell python -c "import sys; print '-I'+sys.prefix+'/include/python'+sys.version[:3]")

python: .python .python/kconfig.py .python/_kconfig.so

.python:
	mkdir .python

.python/kconfig_wrap.c .python/kconfig.py: kconfig.i kconfig_load.c expr.h lkc_proto.h
	swig -python -o .python/kconfig_wrap.c kconfig.i

.python/_kconfig.so: .python/kconfig_wrap.c
	cd .python; $(CC) $(CFLAGS) -shared kconfig_wrap.c -o _kconfig.so -I.. $(PYTHON_INCLUDE)


.PHONY: all tgz clean ruby python
