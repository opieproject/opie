TEMPLATE        = app
CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/bin
HEADERS		= showimg.h
SOURCES		= main.cpp \
		  showimg.cpp
TARGET          = showimg
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe
REQUIRES        = showimg

TRANSLATIONS = ../../../i18n/de/showimg.ts \
	 ../../../i18n/en/showimg.ts \
	 ../../../i18n/es/showimg.ts \
	 ../../../i18n/fr/showimg.ts \
	 ../../../i18n/hu/showimg.ts \
	 ../../../i18n/ja/showimg.ts \
	 ../../../i18n/ko/showimg.ts \
	 ../../../i18n/no/showimg.ts \
	 ../../../i18n/pl/showimg.ts \
	 ../../../i18n/pt/showimg.ts \
	 ../../../i18n/pt_BR/showimg.ts \
	 ../../../i18n/sl/showimg.ts \
	 ../../../i18n/zh_CN/showimg.ts \
	 ../../../i18n/zh_TW/showimg.ts
