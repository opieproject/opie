TEMPLATE        = lib
CONFIG          += qte warn_on debug

HEADERS         = qcolordialog.h       \
                  qsplitter.h          \
                  qinputdialog.h       \
                                       \
                  ocolorpopupmenu.h


SOURCES         = qcolordialog.cpp     \
                  qsplitter.cpp        \
                  qinputdialog.cpp     \
                                       \
                  ocolorpopupmenu.cpp

TARGET          = qtaux2
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/lib
INTERFACES      =


include ( $(OPIEDIR)/include.pro )
