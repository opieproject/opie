TEMPLATE  = app
CONFIG    += qt warn_on release
HEADERS   = formatter.h inputDialog.h output.h
SOURCES   = formatter.cpp inputDialog.cpp output.cpp main.cpp
TARGET    = formatter
DESTDIR   = $(OPIEDIR)/bin
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../../../i18n/de/formatter.ts \
	 ../../../i18n/da/formatter.ts \
	 ../../../i18n/xx/formatter.ts \
	 ../../../i18n/en/formatter.ts \
	 ../../../i18n/es/formatter.ts \
	 ../../../i18n/fr/formatter.ts \
	 ../../../i18n/hu/formatter.ts \
	 ../../../i18n/ja/formatter.ts \
	 ../../../i18n/ko/formatter.ts \
	 ../../../i18n/no/formatter.ts \
	 ../../../i18n/pl/formatter.ts \
	 ../../../i18n/pt/formatter.ts \
	 ../../../i18n/pt_BR/formatter.ts \
	 ../../../i18n/sl/formatter.ts \
	 ../../../i18n/zh_CN/formatter.ts \
	 ../../../i18n/zh_TW/formatter.ts
