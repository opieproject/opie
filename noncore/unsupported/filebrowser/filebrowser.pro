TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= inlineedit.h  filebrowser.h filePermissions.h
SOURCES		= filebrowser.cpp inlineedit.cpp filePermissions.cpp main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	=

TRANSLATIONS = ../../../i18n/de/filebrowser.ts \
	 ../../../i18n/en/filebrowser.ts \
	 ../../../i18n/es/filebrowser.ts \
	 ../../../i18n/fr/filebrowser.ts \
	 ../../../i18n/hu/filebrowser.ts \
	 ../../../i18n/ja/filebrowser.ts \
	 ../../../i18n/ko/filebrowser.ts \
	 ../../../i18n/no/filebrowser.ts \
	 ../../../i18n/pl/filebrowser.ts \
	 ../../../i18n/pt/filebrowser.ts \
	 ../../../i18n/pt_BR/filebrowser.ts \
	 ../../../i18n/sl/filebrowser.ts \
	 ../../../i18n/zh_CN/filebrowser.ts \
	 ../../../i18n/it/filebrowser.ts \
	 ../../../i18n/zh_TW/filebrowser.ts
