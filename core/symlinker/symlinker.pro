TEMPLATE    = app
CONFIG      += qt warn_on
DESTDIR     = $(OPIEDIR)/bin

HEADERS     =
SOURCES     = main.cpp
INTERFACES  =

TARGET      = opie-update-symlinks
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include .
LIBS            += -lqpe -lopiecore2

include ( $(OPIEDIR)/include.pro )

