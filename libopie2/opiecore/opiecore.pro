TEMPLATE    = lib
CONFIG      += qt warn_on
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = oapplication.h            \
              oconfig.h                 \
              odebug.h                  \
              oglobal.h                 \
              oglobalsettings.h         \
              okeyconfigmanager.h       \
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
              okeyconfigmanager.cpp     \
              oprocess.cpp              \
              oprocctrl.cpp             \
              ostorageinfo.cpp          \
              xmltree.cpp


# The following files are currently not compileable on mac !
# Therfore I removed them from the build .. (eilers)

CONFTEST = $$system( echo $CONFIG_TARGET_MACOSX )
!contains( CONFTEST, y ) {
HEADERS     += ofilenotify.h 
SOURCES     += ofilenotify.cpp
} else {
    message( "ofilenotify is not available in a mac build !" )
}

include ( device/device.pro )

INTERFACES  =
TARGET      = opiecore2
VERSION     = 1.9.1
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

!contains( platform, x11 ) {
  LIBS        = -lqpe
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        = -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
}

