TEMPLATE    = lib
CONFIG      += qt warn_on debug
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = ocontact.h ocontactfields.h opimrecord.h otodo.h opimnotify.h opimnotifymanager.h oevent.h
              
SOURCES     = ocontact.cpp ocontactfields.cpp opimrecord.cpp otodo.cpp opimnotify.cpp \
	opimnotifymanager.cpp oevent.cpp
INTERFACES  =
TARGET      = opiepim2
VERSION     = 1.8.2
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

include ( core/core.pro )
include ( core/backends/backends.pro )
include ( ui/ui.pro )

MOC_DIR     = moc
OBJECTS_DIR = obj


!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        = -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib 
}

LIBS += -lopiecore2
