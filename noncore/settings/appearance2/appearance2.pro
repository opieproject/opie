TEMPLATE  = app
CONFIG    = qt warn_on release
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = appearance.h editScheme.h sample.h
SOURCES   = appearance.cpp editScheme.cpp main.cpp sample.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += ../$(OPIEDIR)/include
LIBS            += -lqpe -lopie
TARGET    = appearance
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

TRANSLATIONS = ../../../i18n/de/appearance.ts \
	 ../../../i18n/xx/appearance.ts \
	 ../../../i18n/en/appearance.ts \
	 ../../../i18n/es/appearance.ts \
	 ../../../i18n/fr/appearance.ts \
	 ../../../i18n/hu/appearance.ts \
	 ../../../i18n/ja/appearance.ts \
	 ../../../i18n/ko/appearance.ts \
	 ../../../i18n/no/appearance.ts \
	 ../../../i18n/pl/appearance.ts \
	 ../../../i18n/pt/appearance.ts \
	 ../../../i18n/pt_BR/appearance.ts \
	 ../../../i18n/sl/appearance.ts \
	 ../../../i18n/zh_CN/appearance.ts \
	 ../../../i18n/zh_TW/appearance.ts \
	 ../../../i18n/da/appearance.ts
