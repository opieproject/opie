TEMPLATE = app
CONFIG += qt warn_on

HEADERS         =
SOURCES         = main.cpp

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      +=

LIBS            += -lqpe -lopiecore2 -lopieui2 -lopiesecurity2

DESTDIR = $(OPIEDIR)/bin
TARGET = multiauth

include ( $(OPIEDIR)/include.pro )


