TEMPLATE    = lib
CONFIG      += qt warn_on debug
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = olistview.h            \
              oimageeffect.h         \
              opixmapeffect.h        \
              opopupmenu.h           \
              opixmapprovider.h      \
              oselector.h            \
              oversatileview.h       \
              oversatileviewitem.h   \
              odialog.h              \
              omessagebox.h          \
              oresource.h            \
              otaskbarapplet.h       \
              oseparator.h

SOURCES     = olistview.cpp          \
              oimageeffect.cpp       \
              opixmapeffect.cpp      \
              opopupmenu.cpp         \
              opixmapprovider.cpp    \
              oselector.cpp          \
              oversatileview.cpp     \
              oversatileviewitem.cpp \
              odialog.cpp            \
              oresource.cpp          \
              otaskbarapplet.cpp     \
              oseparator.cpp

INTERFACES  =
TARGET      = opieui2
VERSION     = 1.8.2

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

LIBS        += -lopiecore2

!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
}

