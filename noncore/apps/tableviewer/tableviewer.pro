TEMPLATE	= app
CONFIG		= qt warn_on debug
DESTDIR		= $(OPIEDIR)/bin
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

TRANSLATIONS = ../../../i18n/de/tableviewer.ts \
	 ../../../i18n/en/tableviewer.ts \
	 ../../../i18n/es/tableviewer.ts \
	 ../../../i18n/fr/tableviewer.ts \
	 ../../../i18n/hu/tableviewer.ts \
	 ../../../i18n/ja/tableviewer.ts \
	 ../../../i18n/ko/tableviewer.ts \
	 ../../../i18n/no/tableviewer.ts \
	 ../../../i18n/pl/tableviewer.ts \
	 ../../../i18n/pt/tableviewer.ts \
	 ../../../i18n/pt_BR/tableviewer.ts \
	 ../../../i18n/sl/tableviewer.ts \
	 ../../../i18n/zh_CN/tableviewer.ts \
	 ../../../i18n/zh_TW/tableviewer.ts
