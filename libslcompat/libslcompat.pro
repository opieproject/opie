TEMPLATE        = lib
CONFIG          += qte warn_on 

HEADERS         = slcolorselector.h       \
                  slfileselector.h        \
                  slmisc.h

SOURCES         = slcolorselector.cpp     \
                  slfileselector.cpp

TARGET          = slcompat
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/lib
LIBS        += -lopiecore2 -lopieui2 -lqtaux2

include ( $(OPIEDIR)/include.pro )
