TEMPLATE  = app
CONFIG    = qt warn_on release
HEADERS   = mainwindow.h \
      traninfo.h    \
      graphinfo.h   \
      checkbook.h   \
      transaction.h \
      graph.h
SOURCES   = main.cpp  \
      mainwindow.cpp  \
      traninfo.cpp    \
      graphinfo.cpp   \
      checkbook.cpp   \
      transaction.cpp \
      graph.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS    += -lqpe -lopie
TARGET    = checkbook
DESTDIR   = $(OPIEDIR)/bin

TRANSLATIONS = ../../../i18n/de/checkbook.ts \
   ../../../i18n/en/checkbook.ts    \
   ../../../i18n/es/checkbook.ts    \
   ../../../i18n/fr/checkbook.ts    \
   ../../../i18n/hu/checkbook.ts    \
   ../../../i18n/ja/checkbook.ts    \
   ../../../i18n/ko/checkbook.ts    \
   ../../../i18n/no/checkbook.ts    \
   ../../../i18n/pl/checkbook.ts    \
   ../../../i18n/pt/checkbook.ts    \
   ../../../i18n/pt_BR/checkbook.ts \
   ../../../i18n/sl/checkbook.ts    \
   ../../../i18n/zh_CN/checkbook.ts \
   ../../../i18n/zh_TW/checkbook.ts \
   ../../../i18n/it/checkbook.ts
