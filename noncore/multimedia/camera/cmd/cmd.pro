DESTDIR         = $(OPIEDIR)/bin
TEMPLATE        = app
CONFIG          += qt warn_on 

HEADERS         = capture.h

SOURCES         = capture.cpp

INCLUDEPATH     += $(OPIEDIR)/include ../lib
DEPENDPATH      += $(OPIEDIR)/include ../lib
LIBS            += -lqpe -lopiecore2 -lcamera
INTERFACES      =
TARGET          = capture

include ( $(OPIEDIR)/include.pro )

