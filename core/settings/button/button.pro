TEMPLATE     = app
CONFIG      += qt warn_on release
DESTDIR      = $(OPIEDIR)/bin
HEADERS      = buttonsettings.h \
               buttonutils.h \
               remapdlg.h

SOURCES      = main.cpp \
               buttonsettings.cpp \
               buttonutils.cpp \
               remapdlg.cpp

INTERFACES   = remapdlgbase.ui

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopie
TARGET       = buttonsettings

TRANSLATIONS = ../../../i18n/de/buttonsettings.ts \
	 ../../../i18n/xx/buttonsettings.ts \
	 ../../../i18n/en/buttonsettings.ts \
	 ../../../i18n/es/buttonsettings.ts \
	 ../../../i18n/fr/buttonsettings.ts \
	 ../../../i18n/hu/buttonsettings.ts \
	 ../../../i18n/ja/buttonsettings.ts \
	 ../../../i18n/ko/buttonsettings.ts \
	 ../../../i18n/no/buttonsettings.ts \
	 ../../../i18n/pl/buttonsettings.ts \
	 ../../../i18n/pt/buttonsettings.ts \
	 ../../../i18n/pt_BR/buttonsettings.ts \
	 ../../../i18n/sl/buttonsettings.ts \
	 ../../../i18n/zh_CN/buttonsettings.ts \
	 ../../../i18n/zh_TW/buttonsettings.ts \
	 ../../../i18n/da/buttonsettings.ts
