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


TRANSLATIONS = ../../../i18n/de/tabmanager.ts
TRANSLATIONS += ../../../i18n/en/tabmanager.ts
TRANSLATIONS += ../../../i18n/es/tabmanager.ts
TRANSLATIONS += ../../../i18n/fr/tabmanager.ts
TRANSLATIONS += ../../../i18n/hu/tabmanager.ts
TRANSLATIONS += ../../../i18n/ja/tabmanager.ts
TRANSLATIONS += ../../../i18n/ko/tabmanager.ts
TRANSLATIONS += ../../../i18n/no/tabmanager.ts
TRANSLATIONS += ../../../i18n/pl/tabmanager.ts
TRANSLATIONS += ../../../i18n/pt/tabmanager.ts
TRANSLATIONS += ../../../i18n/pt_BR/tabmanager.ts
TRANSLATIONS += ../../../i18n/sl/tabmanager.ts
TRANSLATIONS += ../../../i18n/zh_CN/tabmanager.ts
TRANSLATIONS += ../../../i18n/zh_TW/tabmanager.ts

