TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= $(OPIEDIR)/include/opie/xmltree.h
SOURCES		= xmltree.cc
TARGET		= opie
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR         = $(QTDIR)/lib$(PROJMAK)
