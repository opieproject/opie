TEMPLATE        = app
CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/bin
HEADERS		= mindbreaker.h
SOURCES		= main.cpp \
              mindbreaker.cpp
TARGET          = mindbreaker
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../../../i18n/de/mindbreaker.ts \
	 ../../../i18n/da/mindbreaker.ts \
	 ../../../i18n/xx/mindbreaker.ts \
	 ../../../i18n/en/mindbreaker.ts \
	 ../../../i18n/es/mindbreaker.ts \
	 ../../../i18n/fr/mindbreaker.ts \
	 ../../../i18n/hu/mindbreaker.ts \
	 ../../../i18n/ja/mindbreaker.ts \
	 ../../../i18n/ko/mindbreaker.ts \
	 ../../../i18n/no/mindbreaker.ts \
	 ../../../i18n/pl/mindbreaker.ts \
	 ../../../i18n/pt/mindbreaker.ts \
	 ../../../i18n/pt_BR/mindbreaker.ts \
	 ../../../i18n/sl/mindbreaker.ts \
	 ../../../i18n/zh_CN/mindbreaker.ts \
	 ../../../i18n/zh_TW/mindbreaker.ts
