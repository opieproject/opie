TEMPLATE  = app
CONFIG    += qt warn_on release
HEADERS   = opieftp.h inputDialog.h ftplib.h
SOURCES   = opieftp.cpp inputDialog.cpp ftplib.c main.cpp
TARGET    = opieftp
REQUIRES=medium-config
DESTDIR   = $(OPIEDIR)/bin
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe



TRANSLATIONS = ../../../i18n/de/opieftp.ts
TRANSLATIONS += ../../../i18n/en/opieftp.ts
TRANSLATIONS += ../../../i18n/es/opieftp.ts
TRANSLATIONS += ../../../i18n/fr/opieftp.ts
TRANSLATIONS += ../../../i18n/hu/opieftp.ts
TRANSLATIONS += ../../../i18n/ja/opieftp.ts
TRANSLATIONS += ../../../i18n/ko/opieftp.ts
TRANSLATIONS += ../../../i18n/no/opieftp.ts
TRANSLATIONS += ../../../i18n/pl/opieftp.ts
TRANSLATIONS += ../../../i18n/pt/opieftp.ts
TRANSLATIONS += ../../../i18n/pt_BR/opieftp.ts
TRANSLATIONS += ../../../i18n/sl/opieftp.ts
TRANSLATIONS += ../../../i18n/zh_CN/opieftp.ts
TRANSLATIONS += ../../../i18n/zh_TW/opieftp.ts

