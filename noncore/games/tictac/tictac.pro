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

TRANSLATIONS = ../../../i18n/de/tictac.ts \
	 ../../../i18n/en/tictac.ts \
	 ../../../i18n/es/tictac.ts \
	 ../../../i18n/fr/tictac.ts \
	 ../../../i18n/hu/tictac.ts \
	 ../../../i18n/ja/tictac.ts \
	 ../../../i18n/ko/tictac.ts \
	 ../../../i18n/no/tictac.ts \
	 ../../../i18n/pl/tictac.ts \
	 ../../../i18n/pt/tictac.ts \
	 ../../../i18n/pt_BR/tictac.ts \
	 ../../../i18n/sl/tictac.ts \
	 ../../../i18n/zh_CN/tictac.ts \
	 ../../../i18n/zh_TW/tictac.ts
