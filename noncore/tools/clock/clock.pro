TEMPLATE  = app
CONFIG    = qt warn_on release
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = clock.h setAlarm.h
SOURCES   = clock.cpp setAlarm.cpp \
      main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lopie
INTERFACES  = 
TARGET    = clock

TRANSLATIONS = ../i18n/de/clock.ts
TRANSLATIONS += ../i18n/es/clock.ts
TRANSLATIONS += ../i18n/pt/clock.ts
TRANSLATIONS += ../i18n/pt_BR/clock.ts
TRANSLATIONS   += ../i18n/en/clock.ts
TRANSLATIONS   += ../i18n/hu/clock.ts
TRANSLATIONS   += ../i18n/sl/clock.ts
TRANSLATIONS   += ../i18n/ja/clock.ts
TRANSLATIONS   += ../i18n/ko/clock.ts
TRANSLATIONS   += ../i18n/fr/clock.ts
TRANSLATIONS   += ../i18n/pl/clock.ts
TRANSLATIONS   += ../i18n/no/clock.ts
TRANSLATIONS   += ../i18n/zh_CN/clock.ts
TRANSLATIONS   += ../i18n/zh_TW/clock.ts
