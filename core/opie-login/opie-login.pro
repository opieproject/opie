TEMPLATE    = app
CONFIG      = qt warn_on debug usepam

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

TRANSLATIONS = ../../i18n/de/opie-login.ts \
·    ../../i18n/en/opie-login.ts \
·    ../../i18n/es/opie-login.ts \
·    ../../i18n/fr/opie-login.ts \
·    ../../i18n/hu/opie-login.ts \
·    ../../i18n/ja/opie-login.ts \
·    ../../i18n/ko/opie-login.ts \
·    ../../i18n/no/opie-login.ts \
·    ../../i18n/pl/opie-login.ts \
·    ../../i18n/pt/opie-login.ts \
·    ../../i18n/pt_BR/opie-login.ts \
·    ../../i18n/sl/opie-login.ts \
·    ../../i18n/zh_CN/opie-login.ts \
·    ../../i18n/zh_TW/opie-login.ts \
·    ../../i18n/it/opie-login.ts \
·    ../../i18n/da/opie-login.ts
	  
