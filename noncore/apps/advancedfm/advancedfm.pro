TEMPLATE  = app
CONFIG    += qt warn_on release
HEADERS   = advancedfm.h inputDialog.h filePermissions.h output.h
SOURCES   = advancedfm.cpp inputDialog.cpp filePermissions.cpp output.cpp main.cpp
TARGET    = advancedfm
REQUIRES=medium-config
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
DESTDIR = $(OPIEDIR)/bin
LIBS            += -lqpe -lopie



TRANSLATIONS = ../../../i18n/de/advancedfm.ts
TRANSLATIONS += ../../../i18n/en/advancedfm.ts
TRANSLATIONS += ../../../i18n/es/advancedfm.ts
TRANSLATIONS += ../../../i18n/fr/advancedfm.ts
TRANSLATIONS += ../../../i18n/hu/advancedfm.ts
TRANSLATIONS += ../../../i18n/ja/advancedfm.ts
TRANSLATIONS += ../../../i18n/ko/advancedfm.ts
TRANSLATIONS += ../../../i18n/no/advancedfm.ts
TRANSLATIONS += ../../../i18n/pl/advancedfm.ts
TRANSLATIONS += ../../../i18n/pt/advancedfm.ts
TRANSLATIONS += ../../../i18n/pt_BR/advancedfm.ts
TRANSLATIONS += ../../../i18n/sl/advancedfm.ts
TRANSLATIONS += ../../../i18n/zh_CN/advancedfm.ts
TRANSLATIONS += ../../../i18n/zh_TW/advancedfm.ts

