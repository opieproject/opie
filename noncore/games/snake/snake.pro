TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= snake.h target.h obstacle.h interface.h codes.h
SOURCES		= snake.cpp target.cpp obstacle.cpp interface.cpp main.cpp
TARGET		= snake
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../../../i18n/de/snake.ts \
	 ../../../i18n/da/snake.ts \
	 ../../../i18n/xx/snake.ts \
	 ../../../i18n/en/snake.ts \
	 ../../../i18n/es/snake.ts \
	 ../../../i18n/fr/snake.ts \
	 ../../../i18n/hu/snake.ts \
	 ../../../i18n/ja/snake.ts \
	 ../../../i18n/ko/snake.ts \
	 ../../../i18n/no/snake.ts \
	 ../../../i18n/pl/snake.ts \
	 ../../../i18n/pt/snake.ts \
	 ../../../i18n/pt_BR/snake.ts \
	 ../../../i18n/sl/snake.ts \
	 ../../../i18n/zh_CN/snake.ts \
	 ../../../i18n/zh_TW/snake.ts
