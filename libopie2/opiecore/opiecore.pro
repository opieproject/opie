TEMPLATE    = lib
CONFIG      += qt warn_on debug
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = oapplication.h            \
              oconfig.h                 \
              odebug.h                  \
              odevice.h                 \
              odevicebutton.h           \
              oglobal.h                 \
              oglobalsettings.h         \
              oprocess.h                \
              oprocctrl.h               \
              ostorageinfo.h

SOURCES     = oapplication.cpp          \
              oconfig.cpp               \
              odebug.cpp                \
              odevice.cpp               \
              odevicebutton.cpp         \
              odevice_ipaq.cpp          \
              odevice_jornada.cpp       \
              odevice_ramses.cpp        \
              odevice_simpad.cpp        \
              odevice_zaurus.cpp        \
              odevice_yopy.cpp          \
              oglobal.cpp               \
              oglobalsettings.cpp       \
              oprocess.cpp              \
              oprocctrl.cpp             \
              ostorageinfo.cpp

INTERFACES  =
TARGET      = opiecore2
VERSION     = 1.8.3
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
MOC_DIR     = moc
OBJECTS_DIR = obj


!contains( platform, x11 ) {
  LIBS        = -lqpe
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        = -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
}

