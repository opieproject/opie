TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS	= addressbook.h \
                  contacteditor.h \
		  ablabel.h \
		  abtable.h \
		  addresssettings.h \
		  picker.h
SOURCES	= main.cpp \
		  addressbook.cpp \
		  contacteditor.cpp \
		  ablabel.cpp \
		  abtable.cpp \
		  addresssettings.cpp \
		  picker.cpp
INTERFACES	= addresssettingsbase.ui

TARGET		= addressbook
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie

TRANSLATIONS    = ../../i18n/pt_BR/addressbook.ts
TRANSLATIONS    += ../../i18n/es/addressbook.ts
TRANSLATIONS    += ../../i18n/pt/addressbook.ts
TRANSLATIONS   += ../../i18n/de/addressbook.ts
TRANSLATIONS   += ../../i18n/en/addressbook.ts
TRANSLATIONS   += ../../i18n/hu/addressbook.ts
TRANSLATIONS   += ../../i18n/sl/addressbook.ts
TRANSLATIONS   += ../../i18n/ja/addressbook.ts
TRANSLATIONS   += ../../i18n/ko/addressbook.ts
TRANSLATIONS   += ../../i18n/pl/addressbook.ts
TRANSLATIONS   += ../../i18n/no/addressbook.ts
TRANSLATIONS   += ../../i18n/zh_CN/addressbook.ts
TRANSLATIONS   += ../../i18n/zh_TW/addressbook.ts
TRANSLATIONS   += ../../i18n/fr/addressbook.ts
