CONFIG += qt warn_on release quick-app
HEADERS = citytime.h citytimebase.h zonemap.h sun.h stylusnormalizer.h
SOURCES = citytime.cpp citytimebase.cpp zonemap.cpp main.cpp sun.c stylusnormalizer.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe

TARGET	= citytime

TRANSLATIONS = ../../../i18n/de/citytime.ts \
	 ../../../i18n/nl/citytime.ts \
	 ../../../i18n/xx/citytime.ts \
	 ../../../i18n/en/citytime.ts \
	 ../../../i18n/es/citytime.ts \
	 ../../../i18n/fr/citytime.ts \
	 ../../../i18n/hu/citytime.ts \
	 ../../../i18n/ja/citytime.ts \
	 ../../../i18n/ko/citytime.ts \
	 ../../../i18n/no/citytime.ts \
	 ../../../i18n/pl/citytime.ts \
	 ../../../i18n/pt/citytime.ts \
	 ../../../i18n/pt_BR/citytime.ts \
	 ../../../i18n/sl/citytime.ts \
	 ../../../i18n/zh_CN/citytime.ts \
	 ../../../i18n/zh_TW/citytime.ts \
	 ../../../i18n/da/citytime.ts



include ( $(OPIEDIR)/include.pro )
