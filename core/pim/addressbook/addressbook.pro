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

TRANSLATIONS = ../../../i18n/de/addressbook.ts \
	 ../../../i18n/en/addressbook.ts \
	 ../../../i18n/es/addressbook.ts \
	 ../../../i18n/fr/addressbook.ts \
	 ../../../i18n/hu/addressbook.ts \
	 ../../../i18n/ja/addressbook.ts \
	 ../../../i18n/ko/addressbook.ts \
	 ../../../i18n/no/addressbook.ts \
	 ../../../i18n/pl/addressbook.ts \
	 ../../../i18n/pt/addressbook.ts \
	 ../../../i18n/pt_BR/addressbook.ts \
	 ../../../i18n/sl/addressbook.ts \
	 ../../../i18n/zh_CN/addressbook.ts \
	 ../../../i18n/zh_TW/addressbook.ts
