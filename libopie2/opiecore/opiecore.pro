TEMPLATE    = lib
CONFIG      += qt warn_on debug
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = oapplication.h            \
              oconfig.h                 \
              odebug.h                  \
              oglobal.h                 \
              oglobalsettings.h         \
              oprocess.h                \
              oprocctrl.h               \
              osmartpointer.h           \
              ostorageinfo.h            \
              xmltree.h

SOURCES     = oapplication.cpp          \
              oconfig.cpp               \
              odebug.cpp                \
              oglobal.cpp               \
              oglobalsettings.cpp       \
              oprocess.cpp              \
              oprocctrl.cpp             \
              ostorageinfo.cpp          \
              xmltree.cc

include ( device/device.pro )

INTERFACES  =
TARGET      = opiecore2
VERSION     = 1.8.5
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

!contains( platform, x11 ) {
  LIBS        = -lqpe
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        = -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
}

