TEMPLATE	=app
CONFIG		+=qt warn_on release
DESTDIR		=$(OPIEDIR)/bin
HEADERS		= remotetab.h learntab.h configtab.h topgroup.h dvdgroup.h channelgroup.h vcrgroup.h buttondialog.h topgroupconf.h dvdgroupconf.h channelgroupconf.h vcrgroupconf.h mainview.h recorddialog.h helptab.h
SOURCES		=remote.cpp remotetab.cpp learntab.cpp configtab.cpp topgroup.cpp dvdgroup.cpp channelgroup.cpp vcrgroup.cpp buttondialog.cpp topgroupconf.cpp dvdgroupconf.cpp channelgroupconf.cpp vcrgroupconf.cpp mainview.cpp recorddialog.cpp helptab.cpp
INCLUDEPATH	+=$(OPIEDIR)/include
DEPENDPATH	+=$(OPIEDIR)/include
LIBS		+=-lqpe -lopie
TARGET          = remote

TRANSLATIONS = ../../../i18n/de/remote.ts \
	 ../../../i18n/da/remote.ts \
	 ../../../i18n/xx/remote.ts \
	 ../../../i18n/en/remote.ts \
	 ../../../i18n/es/remote.ts \
	 ../../../i18n/fr/remote.ts \
	 ../../../i18n/hu/remote.ts \
	 ../../../i18n/ja/remote.ts \
	 ../../../i18n/ko/remote.ts \
	 ../../../i18n/no/remote.ts \
	 ../../../i18n/pl/remote.ts \
	 ../../../i18n/pt/remote.ts \
	 ../../../i18n/pt_BR/remote.ts \
	 ../../../i18n/sl/remote.ts \
	 ../../../i18n/zh_CN/remote.ts \
	 ../../../i18n/zh_TW/remote.ts
