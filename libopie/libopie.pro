TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= $(OPIEDIR)/include/opie/xmltree.h
SOURCES		= xmltree.cc tododb.cpp todoevent.cpp todovcalresource.cpp
TARGET		= opie
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR         = $(QTDIR)/lib$(PROJMAK)
