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
              oseparator.h           \
              otaskbarapplet.h

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
              oseparator.cpp         \
              otaskbarapplet.cpp

INTERFACES  =
TARGET      = opieui2
VERSION     = 1.8.1
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lopiecore2
MOC_DIR     = moc
OBJECTS_DIR = obj

include ( $(OPIEDIR)/include.pro )

