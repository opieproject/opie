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
	      omenubar.h             \
	      opiemenubar.h          \
	      omessagebox.h          \
	      oresource.h            \
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
	      omenubar.cpp           \
	      opiemenubar.cpp        \
	      oresource.cpp          \
              oseparator.cpp         

INTERFACES  =
TARGET      = opieui2
VERSION     = 1.8.2
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lopiecore2
MOC_DIR     = moc
OBJECTS_DIR = obj


!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
  HEADERS     += otaskbarapplet.h
  SOURCES     += otaskbarapplet.cpp
}

contains( platform, x11 ) {
  LIBS        += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib 
  message( Warning: NO otaskbarapplet ATM )
}

