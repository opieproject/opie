TEMPLATE    = lib
CONFIG      += qt warn_on debug
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = opimcontact.h opimcontactfields.h opimrecord.h opimtodo.h opimnotify.h \
              opimnotifymanager.h opimevent.h

SOURCES     = opimcontact.cpp opimcontactfields.cpp opimrecord.cpp opimtodo.cpp opimnotify.cpp \
              opimnotifymanager.cpp opimevent.cpp
INTERFACES  =
TARGET      = opiepim2
VERSION     = 1.8.3
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lopiecore2 -lopiedb2

include ( core/core.pro )
include ( core/backends/backends.pro )
include ( ui/ui.pro )

!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        = -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib 
}

