MOC_DIR         = ./moc
OBJECTS_DIR     = ./obj
DESTDIR         = $(OPIEDIR)/bin
TEMPLATE        = app
CONFIG          = qt warn_on debug

HEADERS         = 

SOURCES         = capture.cpp

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
INTERFACES      =
TARGET          = capture

include ( $(OPIEDIR)/include.pro )

