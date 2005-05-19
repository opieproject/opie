TEMPLATE    = lib
CONFIG      += qt warn_on
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = oapplication.h            \
              oconfig.h                 \
              odebug.h                  \
              oglobal.h                 \
              oglobalsettings.h         \
              okeyconfigmanager.h       \
              okeyfilter.h              \
              opluginloader.h           \
              oprocess.h                \
              oprocctrl.h               \
              oresource.h               \
              osharedpointer.h          \
              osmartpointer.h           \
              ostorageinfo.h            \
              xmltree.h

SOURCES     = oapplication.cpp          \
              oconfig.cpp               \
              odebug.cpp                \
              oglobal.cpp               \
              oglobalsettings.cpp       \
              okeyconfigmanager.cpp     \
              okeyfilter.cpp            \
              opluginloader.cpp         \
              oprocess.cpp              \
              oprocctrl.cpp             \
              oresource.cpp             \
              osmartpointer.cpp         \
              ostorageinfo.cpp          \
              xmltree.cpp


# Disable Linux-only subsystems for MAC build
!contains( CONFIG_TARGET_MACOSX, y ) {
include ( linux/linux.pro )
} else {
    message( "disabling linux-only subsystems for MAC build" )
}

include( device/device.pro )

INTERFACES  =
TARGET      = opiecore2
VERSION     = 1.9.4
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

!contains( platform, x11 ) {
  LIBS        = -lqpe
  include( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        = -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
}

