TEMPLATE  = app
CONFIG    += qt warn_on release
HEADERS   = tictac.h
SOURCES   = main.cpp \
      tictac.cpp
TARGET    = tictac
REQUIRES=medium-config
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe
DESTDIR   = $(OPIEDIR)/bin

TRANSLATIONS    = ../../i18n/pt_BR/tictac.ts
TRANSLATIONS   += ../../i18n/de/tictac.ts
TRANSLATIONS   += ../../i18n/en/tictac.ts
TRANSLATIONS   += ../../i18n/hu/tictac.ts
TRANSLATIONS   += ../../i18n/sl/tictac.ts
TRANSLATIONS   += ../../i18n/ja/tictac.ts
TRANSLATIONS   += ../../i18n/ko/tictac.ts
TRANSLATIONS   += ../../i18n/no/tictac.ts
TRANSLATIONS   += ../../i18n/pl/tictac.ts
TRANSLATIONS   += ../../i18n/zh_CN/tictac.ts
TRANSLATIONS   += ../../i18n/zh_TW/tictac.ts
TRANSLATIONS   += ../../i18n/fr/tictac.ts
