TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= $(OPIEDIR)/include/opie/xmltree.h
SOURCES		= xmltree.cc tododb.cpp todoevent.cpp
TARGET		= opie
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR         = $(QTDIR)/lib$(PROJMAK)
