TEMPLATE        = lib

HEADERS         = ocolorpopupmenu.h    \
                  ocolorbutton.h       \
                  oticker.h


SOURCES         = ocolorpopupmenu.cpp  \
                  ocolorbutton.cpp     \
                  oticker.cpp

TARGET          = qtaux2
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/lib


include( $(OPIEDIR)/include.pro )
