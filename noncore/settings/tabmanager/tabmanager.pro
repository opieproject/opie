TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS		= tabmanager.h tabmainwindow.h app.h tablistview.h tabmanagerbase.h tabapplnk.h
SOURCES		= main.cpp tabmanager.cpp app.cpp tabmanagerbase.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
#INTERFACES	= tabmanagerbase.ui
TARGET		= tabmanager
DESTDIR 	= $(OPIEDIR)/bin

TRANSLATIONS = ../../../i18n/de/tabmanager.ts \
	 ../../../i18n/en/tabmanager.ts \
	 ../../../i18n/es/tabmanager.ts \
	 ../../../i18n/fr/tabmanager.ts \
	 ../../../i18n/hu/tabmanager.ts \
	 ../../../i18n/ja/tabmanager.ts \
	 ../../../i18n/ko/tabmanager.ts \
	 ../../../i18n/no/tabmanager.ts \
	 ../../../i18n/pl/tabmanager.ts \
	 ../../../i18n/pt/tabmanager.ts \
	 ../../../i18n/pt_BR/tabmanager.ts \
	 ../../../i18n/sl/tabmanager.ts \
	 ../../../i18n/zh_CN/tabmanager.ts \
	 ../../../i18n/zh_TW/tabmanager.ts
