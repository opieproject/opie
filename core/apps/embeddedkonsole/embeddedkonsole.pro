TEMPLATE  = app
CONFIG    += qt warn_on release
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = TEWidget.h \
      TEScreen.h \
      TECommon.h \
      TEHistory.h \
      TEmulation.h \
      TEmuVt102.h \
      session.h \
      keytrans.h \
      konsole.h \
      commandeditdialog.h \
      commandeditwidget.h \
      playlistselection.h \
      MyPty.h
SOURCES   = TEScreen.cpp \
      TEWidget.cpp \
      TEHistory.cpp \
      TEmulation.cpp \
      TEmuVt102.cpp \
      session.cpp \
      keytrans.cpp \
      konsole.cpp \
      commandeditdialog.cpp \
      commandeditwidget.cpp \
      playlistselection.cpp \
      MyPty.cpp \
      main.cpp 
INTERFACES =  commandeditdialogbase.ui smallcommandeditdialogbase.ui
TARGET    = embeddedkonsole
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe
REQUIRES  = embeddedkonsole

TRANSLATIONS = ../i18n/pt_BR/embeddedkonsole.ts
TRANSLATIONS   += ../i18n/de/embeddedkonsole.ts
TRANSLATIONS   += ../i18n/en/embeddedkonsole.ts
TRANSLATIONS   += ../i18n/hu/embeddedkonsole.ts
TRANSLATIONS   += ../i18n/fr/embeddedkonsole.ts
TRANSLATIONS   += ../i18n/ja/embeddedkonsole.ts
TRANSLATIONS   += ../i18n/ko/embeddedkonsole.ts
TRANSLATIONS   += ../i18n/no/embeddedkonsole.ts
TRANSLATIONS   += ../i18n/sl/embeddedkonsole.ts
TRANSLATIONS   += ../i18n/zh_CN/embeddedkonsole.ts
TRANSLATIONS   += ../i18n/zh_TW/embeddedkonsole.ts
