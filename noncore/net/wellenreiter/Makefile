# Warning: Handcrafted!

#############################################################################
# Makefile for building targets in sub directories.
#     Project: wellenreiter
#    Template: subdirs
#############################################################################

MAKEFILE=	Makefile
TMAKE	=	tmake

SUBDIRS	=	libwellenreiter/source daemon/source gui

all: $(SUBDIRS)

$(SUBDIRS): FORCE
	cd $@; $(MAKE)

tmake:
	tmake wellenreiter.pro

tmake_all:
	for i in $(SUBDIRS); do ( if [ -d $$i ]; then cd $$i ; pro=`basename $$i`.pro ; $(TMAKE) $$pro -o $(MAKEFILE); grep "TEMPLATE.*subdirs" $$pro 2>/dev/null >/dev/null && $(MAKE) -f $(MAKEFILE) tmake_all || true; fi; ) ; done

clean release debug:
	for i in $(SUBDIRS); do ( if [ -d $$i ]; then cd $$i ; $(MAKE) $@; fi; ) ; done

FORCE:
