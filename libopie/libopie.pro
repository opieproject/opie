TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= ofileselector.h tododb.h todoevent.h todoresource.h todovcalresource.h xmltree.h oconfig.h
SOURCES		= ofileselector.cc xmltree.cc tododb.cpp todoevent.cpp todovcalresource.cpp oconfig.cpp
TARGET		= opie
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR         = $(QTDIR)/lib$(PROJMAK)
#VERSION = 1.0.0