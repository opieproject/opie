TEMPLATE    = lib
CONFIG      += qt warn_on 
DESTDIR     = $(OPIEDIR)/lib

INTERFACES  =
TARGET      = opiepim2
VERSION     = 1.9.1
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lopiecore2

include ( $(OPIEDIR)/gen.pro )
include ( core/core.pro )
include ( backend/backends.pro )
include ( ui/ui.pro )
include ( private/private.pro )

PRECOMPILED_HEADER = stable_pch.h

!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        = -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib 
}
