MOC_DIR         = ./moc
OBJECTS_DIR     = ./obj
DESTDIR         = $(OPIEDIR)/bin
TEMPLATE        = app
CONFIG          = qt warn_on debug

HEADERS         = 

SOURCES         = capture.cpp

INCLUDEPATH     += $(OPIEDIR)/include ../lib
DEPENDPATH      += $(OPIEDIR)/include ../lib
LIBS            += -lqpe -lopiecore2 -lopiecam
INTERFACES      =
TARGET          = capture

include ( $(OPIEDIR)/include.pro )

