TEMPLATE    = lib
CONFIG      += qt warn_on debug
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = oapplication.h            \
              oconfig.h                 \
              odebug.h                  \
              oglobal.h                 \
              oglobalsettings.h         \
              ostorage.h

SOURCES     = oapplication.cpp          \
              oconfig.cpp               \
              odebug.cpp                \
              oglobal.cpp               \
              oglobalsettings.cpp       \
              ostorage.cpp

INTERFACES  =
TARGET      = opiecore2
VERSION     = 1.8.2
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

