TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin

HEADERS		= canvascard.h   canvasshapes.h   cardgame.h   cardgamelayout.h   cardpile.h   card.h   carddeck.h   canvascardgame.h   freecellcardgame.h   chicanecardgame.h   harpcardgame.h   patiencecardgame.h   canvascardwindow.h

SOURCES		= canvascard.cpp canvasshapes.cpp cardgame.cpp cardgamelayout.cpp cardpile.cpp card.cpp carddeck.cpp canvascardgame.cpp freecellcardgame.cpp chicanecardgame.cpp harpcardgame.cpp patiencecardgame.cpp canvascardwindow.cpp main.cpp

TARGET		= patience
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
REQUIRES	= patience

TRANSLATIONS = ../../../i18n/de/patience.ts \
	 ../../../i18n/en/patience.ts \
	 ../../../i18n/es/patience.ts \
	 ../../../i18n/fr/patience.ts \
	 ../../../i18n/hu/patience.ts \
	 ../../../i18n/ja/patience.ts \
	 ../../../i18n/ko/patience.ts \
	 ../../../i18n/no/patience.ts \
	 ../../../i18n/pl/patience.ts \
	 ../../../i18n/pt/patience.ts \
	 ../../../i18n/pt_BR/patience.ts \
	 ../../../i18n/sl/patience.ts \
	 ../../../i18n/zh_CN/patience.ts \
	 ../../../i18n/zh_TW/patience.ts
