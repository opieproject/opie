TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS	= addressbook.h \
                  contacteditor.h \
		  ablabel.h \
		  abtable.h \
		  addresssettings.h
SOURCES	= main.cpp \
		  addressbook.cpp \
		  contacteditor.cpp \
		  ablabel.cpp \
		  abtable.cpp \
		  addresssettings.cpp
INTERFACES	= addresssettingsbase.ui

TARGET		= addressbook
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS    = ../../i18n/pt_BR/addessbook.ts
TRANSLATIONS   += ../../i18n/de/addessbook.ts
TRANSLATIONS   += ../../i18n/en/addessbook.ts
TRANSLATIONS   += ../../i18n/hu/addessbook.ts
TRANSLATIONS   += ../../i18n/sl/addessbook.ts
TRANSLATIONS   += ../../i18n/ja/addessbook.ts
TRANSLATIONS   += ../../i18n/ko/addessbook.ts
TRANSLATIONS   += ../../i18n/no/addessbook.ts
TRANSLATIONS   += ../../i18n/zh_CN/addessbook.ts
TRANSLATIONS   += ../../i18n/zh_TW/addessbook.ts
TRANSLATIONS   += ../../i18n/fr/addessbook.ts
