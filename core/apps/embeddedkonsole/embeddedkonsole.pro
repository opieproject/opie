CONFIG    += qt warn_on  quick-app
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
LIBS            += -lqpe -lopiecore2 -lqtaux2 -lutil
DEFINES += QT_QWS_OPIE HAVE_OPENPTY

include ( $(OPIEDIR)/include.pro )
