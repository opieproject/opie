TEMPLATE  = app
CONFIG    = qt warn_on 
HEADERS   = inputDialog.h helpwindow.h
SOURCES   = inputDialog.cpp helpwindow.cpp main.cpp
INTERFACES  =
TARGET    = stocktickertest
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS+= -lqpe -lopiecore2
DESTDIR = $(OPIEDIR)/bin

include ( $(OPIEDIR)/include.pro )
