TEMPLATE    = app
CONFIG      = qt warn_on

HEADERS     = loginwindowimpl.h  \
              loginapplication.h \
	      passworddialogimpl.h   \
              ../launcher/inputmethods.h \
              ../apps/calibrate/calibrate.h

SOURCES     = loginwindowimpl.cpp  \
              loginapplication.cpp \
              passworddialogimpl.cpp   \
              ../launcher/inputmethods.cpp \
              ../apps/calibrate/calibrate.cpp \
              main.cpp

INTERFACES  = loginwindow.ui passworddialog.ui

INCLUDEPATH += $(OPIEDIR)/include ../launcher ../apps/calibrate
DEPENDPATH  += $(OPIEDIR)/include ../launcher ../apps/calibrate

LIBS        += -lqpe -lopiecore2

DESTDIR     = $(OPIEDIR)/bin
TARGET      = opie-login

include(



























 $(OPIEDIR)/include.pro )

OPIELOGIN_USEPAM:LIBS += -lpam
OPIELOGIN_USEPAM:DEFINES += USEPAM
