TEMPLATE	= app
CONFIG		= qt warn_on debug
DESTDIR		= ../bin
SUBDIRS		= db ui
HEADERS		= tableviewer.h \
		  ui/commonwidgets.h \
		  ui/tvbrowseview.h \
		  ui/tvlistview.h \
		  ui/tvfilterview.h \
		  ui/tveditview.h \
		  ui/browsekeyentry.h \
		  ui/filterkeyentry.h \
		  ui/tvkeyedit.h \
		  db/datacache.h \
		  db/common.h \
		  db/xmlsource.h \
		  db/csvsource.h
SOURCES		= main.cpp \
		  tableviewer.cpp \
		  ui/commonwidgets.cpp \
		  ui/tvbrowseview.cpp \
		  ui/tvfilterview.cpp \
		  ui/browsekeyentry.cpp \
		  ui/filterkeyentry.cpp \
		  ui/tvlistview.cpp \
		  ui/tveditview.cpp \
		  ui/tvkeyedit.cpp \
		  db/datacache.cpp \
		  db/xmlsource.cpp \
		  db/csvsource.cpp \
		  db/common.cpp
INTERFACES	= ui/tvkeyedit_gen.ui
TARGET		= tableviewer
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH     += $(OPIEDIR)/include
LIBS	       += -lqpe

TRANSLATIONS = ../i18n/pt_BR/tableviewer.ts
TRANSLATIONS += ../i18n/es/tableviewer.ts
TRANSLATIONS += ../i18n/pt/tableviewer.ts
TRANSLATIONS   += ../i18n/en/tableviewer.ts
TRANSLATIONS   += ../i18n/de/tableviewer.ts
TRANSLATIONS   += ../i18n/hu/tableviewer.ts
TRANSLATIONS   += ../i18n/pl/tableviewer.ts
TRANSLATIONS   += ../i18n/ja/tableviewer.ts
TRANSLATIONS   += ../i18n/sl/tableviewer.ts
TRANSLATIONS   += ../i18n/fr/tableviewer.ts
TRANSLATIONS   += ../i18n/ko/tableviewer.ts
TRANSLATIONS   += ../i18n/no/tableviewer.ts
TRANSLATIONS   += ../i18n/zh_CN/tableviewer.ts
TRANSLATIONS   += ../i18n/zh_TW/tableviewer.ts
