TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= inlineedit.h  filebrowser.h filePermissions.h
SOURCES		= filebrowser.cpp inlineedit.cpp filePermissions.cpp main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	=

TRANSLATIONS = ../../../i18n/de/filebrowser.pro.in.ts \
	 ../../../i18n/en/filebrowser.pro.in.ts \
	 ../../../i18n/es/filebrowser.pro.in.ts \
	 ../../../i18n/fr/filebrowser.pro.in.ts \
	 ../../../i18n/hu/filebrowser.pro.in.ts \
	 ../../../i18n/ja/filebrowser.pro.in.ts \
	 ../../../i18n/ko/filebrowser.pro.in.ts \
	 ../../../i18n/no/filebrowser.pro.in.ts \
	 ../../../i18n/pl/filebrowser.pro.in.ts \
	 ../../../i18n/pt/filebrowser.pro.in.ts \
	 ../../../i18n/pt_BR/filebrowser.pro.in.ts \
	 ../../../i18n/sl/filebrowser.pro.in.ts \
	 ../../../i18n/zh_CN/filebrowser.pro.in.ts \
	 ../../../i18n/zh_TW/filebrowser.pro.in.ts
