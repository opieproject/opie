TEMPLATE    = lib
CONFIG      += qt warn_on debug
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = ocompletionbox.h       \
              ocombobox.h            \
              oeditlistbox.h         \
              olineedit.h            \
              olistview.h            \
              oimageeffect.h         \
              opixmapeffect.h        \
              opopupmenu.h           \
              opixmapprovider.h      \
              oselector.h            \
              oversatileview.h       \
              oversatileviewitem.h   \
              #ojanuswidget.h         \
              odialog.h              \
	      omenubar.h             \
	      opiemenubar.h          \
	      omessagebox.h          \
	      oresource.h            \
	      otoolbar.h            \
              oseparator.h           
#              otaskbarapplet.h

SOURCES     = ocompletionbox.cpp     \
              ocombobox.cpp          \
              oeditlistbox.cpp       \
              olineedit.cpp          \
              olistview.cpp          \
              oimageeffect.cpp       \
              opixmapeffect.cpp      \
              opopupmenu.cpp         \
              opixmapprovider.cpp    \
              oselector.cpp          \
              oversatileview.cpp     \
              oversatileviewitem.cpp \
              #ojanuswidget.cpp       \
              odialog.cpp            \
	      omenubar.cpp           \
	      opiemenubar.cpp        \
	      oresource.cpp          \
	      otoolbar.cpp           \
              oseparator.cpp         #\
#              otaskbarapplet.cpp

INTERFACES  =
TARGET      = opieui2
VERSION     = 1.8.1
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

