TEMPLATE    = lib
CONFIG      += qt warn_on debug
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = ocheckitem.h           \
              oclickablelabel.h      \
              odialog.h              \
              ofileselector.h        \
              ofileselector_p.h      \
              ofiledialog.h          \
              ofileview.h            \
              oimageeffect.h         \
              olistview.h            \
              opixmapeffect.h        \
              opopupmenu.h           \
              opixmapprovider.h      \
              oselector.h            \
              oversatileview.h       \
              oversatileviewitem.h   \
              omessagebox.h          \
              oresource.h            \
              otaskbarapplet.h       \
              oseparator.h

SOURCES     = ocheckitem.cpp         \
              oclickablelabel.cpp    \
              ofileselector.cpp      \
              ofiledialog.cpp        \
              oimageeffect.cpp       \
              olistview.cpp          \
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
VERSION     = 1.8.3

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

LIBS        += -lopiecore2

!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
}

