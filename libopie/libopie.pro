TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= ofontmenu.h ofileselector.h ofiledialog.h tododb.h todoevent.h todoresource.h todovcalresource.h xmltree.h oconfig.h colordialog.h colorpopupmenu.h
SOURCES		= ofontmenu.cc ofileselector.cc ofiledialog.cc xmltree.cc tododb.cpp todoevent.cpp todovcalresource.cpp oconfig.cpp colordialog.cpp colorpopupmenu.cpp
TARGET		= opie
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR         = $(QTDIR)/lib$(PROJMAK)
#VERSION = 1.0.0
