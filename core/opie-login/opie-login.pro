TEMPLATE    = app
CONFIG      = qt warn_on

HEADERS     = loginwindowimpl.h \
              loginapplication.h \
              ../launcher/inputmethods.h \
              ../apps/calibrate/calibrate.h

SOURCES     = loginwindowimpl.cpp \
              loginapplication.cpp \
              ../launcher/inputmethods.cpp \
              ../apps/calibrate/calibrate.cpp \
              main.cpp

INTERFACES  = loginwindow.ui

INCLUDEPATH += $(OPIEDIR)/include ../launcher ../apps/calibrate
DEPENDPATH  += $(OPIEDIR)/include ../launcher ../apps/calibrate

LIBS        += -lqpe -lopie

usepam:LIBS += -lpam
usepam:DEFINES += USEPAM

DESTDIR     = $(OPIEDIR)/bin
TARGET      = opie-login

