TEMPLATE  = app
CONFIG    += qt warn_on release
HEADERS   = advancedfm.h filePermissions.h output.h
SOURCES   = advancedfm.cpp advancedfmData.cpp advancedfmMenu.cpp filePermissions.cpp output.cpp main.cpp
TARGET    = advancedfm
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
DESTDIR = $(OPIEDIR)/bin
LIBS            += -lqpe -lopie

TRANSLATIONS = ../../../i18n/de/advancedfm.ts \
	 ../../../i18n/da/advancedfm.ts \
	 ../../../i18n/xx/advancedfm.ts \
   ../../../i18n/en/advancedfm.ts \
   ../../../i18n/es/advancedfm.ts \
   ../../../i18n/fr/advancedfm.ts \
   ../../../i18n/hu/advancedfm.ts \
   ../../../i18n/ja/advancedfm.ts \
   ../../../i18n/ko/advancedfm.ts \
   ../../../i18n/no/advancedfm.ts \
   ../../../i18n/pl/advancedfm.ts \
   ../../../i18n/pt/advancedfm.ts \
   ../../../i18n/pt_BR/advancedfm.ts \
   ../../../i18n/sl/advancedfm.ts \
   ../../../i18n/zh_CN/advancedfm.ts \
   ../../../i18n/zh_TW/advancedfm.ts
