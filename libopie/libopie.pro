TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= tododb.h todoevent.h todoresource.h todovcalresource.h xmltree.h
SOURCES		= xmltree.cc tododb.cpp todoevent.cpp todovcalresource.cpp
TARGET		= opie
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR         = $(QTDIR)/lib$(PROJMAK)
