TEMPLATE	= app
#CONFIG		= qt warn_on release
CONFIG		= qt warn_on debug
DESTDIR		= $(OPIEDIR)/bin
HEADERS	= addressbook.h \
                  contacteditor.h \
                  ocontactfields.h \
		  ablabel.h \
		  abtable.h \
		  picker.h \
                  ofloatbar.h \
                  configdlg.h \
                  abconfig.h \
                  abview.h
SOURCES	= main.cpp \
		  addressbook.cpp \
		  contacteditor.cpp \
                  ocontactfields.cpp \
		  ablabel.cpp \
		  abtable.cpp \
		  picker.cpp \
                  configdlg.cpp \
                  abconfig.cpp \
                  abview.cpp

INTERFACES	= configdlg_base.ui
TARGET		= addressbook
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie

TRANSLATIONS = ../../../i18n/de/addressbook.ts \
	 ../../../i18n/da/addressbook.ts \
	 ../../../i18n/xx/addressbook.ts \
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
	 ../../../i18n/it/addressbook.ts \
	 ../../../i18n/zh_TW/addressbook.ts
