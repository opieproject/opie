TEMPLATE	=app
CONFIG		+=qt warn_on release
DESTDIR		=$(OPIEDIR)/bin
HEADERS		= remotetab.h learntab.h configtab.h topgroup.h dvdgroup.h channelgroup.h vcrgroup.h buttondialog.h topgroupconf.h dvdgroupconf.h channelgroupconf.h vcrgroupconf.h mainview.h
SOURCES		=remote.cpp remotetab.cpp learntab.cpp configtab.cpp topgroup.cpp dvdgroup.cpp channelgroup.cpp vcrgroup.cpp buttondialog.cpp topgroupconf.cpp dvdgroupconf.cpp channelgroupconf.cpp vcrgroupconf.cpp mainview.cpp
INCLUDEPATH	+=$(OPIEDIR)/include
DEPENDPATH	+=$(OPIEDIR)/include
LIBS		+=-lqpe
TARGET          = remote


TRANSLATIONS = ../../../i18n/de/remote.ts
TRANSLATIONS += ../../../i18n/en/remote.ts
TRANSLATIONS += ../../../i18n/es/remote.ts
TRANSLATIONS += ../../../i18n/fr/remote.ts
TRANSLATIONS += ../../../i18n/hu/remote.ts
TRANSLATIONS += ../../../i18n/ja/remote.ts
TRANSLATIONS += ../../../i18n/ko/remote.ts
TRANSLATIONS += ../../../i18n/no/remote.ts
TRANSLATIONS += ../../../i18n/pl/remote.ts
TRANSLATIONS += ../../../i18n/pt/remote.ts
TRANSLATIONS += ../../../i18n/pt_BR/remote.ts
TRANSLATIONS += ../../../i18n/sl/remote.ts
TRANSLATIONS += ../../../i18n/zh_CN/remote.ts
TRANSLATIONS += ../../../i18n/zh_TW/remote.ts

