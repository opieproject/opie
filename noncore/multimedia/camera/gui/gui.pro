DESTDIR         = $(OPIEDIR)/bin
TEMPLATE        = app
CONFIG          += qt warn_on debug

HEADERS         = previewwidget.h \
                  mainwindow.h

SOURCES         = previewwidget.cpp \
                  mainwindow.cpp \
                  main.cpp

INCLUDEPATH     += $(OPIEDIR)/include ../lib
DEPENDPATH      += $(OPIEDIR)/include ../lib
LIBS            += -lopiecore2 -lopieui2 -lopiecam
INTERFACES      =
TARGET          = opiecam

include ( $(OPIEDIR)/include.pro )

