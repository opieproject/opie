TEMPLATE  = app
CONFIG    += qt warn_on release
HEADERS   = formatter.h inputDialog.h output.h
SOURCES   = formatter.cpp inputDialog.cpp output.cpp main.cpp
TARGET    = formatter
REQUIRES = medium-config
DESTDIR   = $(OPIEDIR)/bin
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe


TRANSLATIONS = ../../../i18n/de/formatter.ts
TRANSLATIONS += ../../../i18n/en/formatter.ts
TRANSLATIONS += ../../../i18n/es/formatter.ts
TRANSLATIONS += ../../../i18n/fr/formatter.ts
TRANSLATIONS += ../../../i18n/hu/formatter.ts
TRANSLATIONS += ../../../i18n/ja/formatter.ts
TRANSLATIONS += ../../../i18n/ko/formatter.ts
TRANSLATIONS += ../../../i18n/no/formatter.ts
TRANSLATIONS += ../../../i18n/pl/formatter.ts
TRANSLATIONS += ../../../i18n/pt/formatter.ts
TRANSLATIONS += ../../../i18n/pt_BR/formatter.ts
TRANSLATIONS += ../../../i18n/sl/formatter.ts
TRANSLATIONS += ../../../i18n/zh_CN/formatter.ts
TRANSLATIONS += ../../../i18n/zh_TW/formatter.ts

