TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= parser.h device.h manager.h remotedevice.h services.h
SOURCES		= parser.cc device.cc manager.cc remotedevice.cc services.cc
TARGET		= opietooth
INCLUDEPATH += $(OPIEDIR)/include .
DESTDIR         = $(QTDIR)/lib$(PROJMAK)
LIBS += -lopie
#VERSION = 0.0.0


TRANSLATIONS = ../../../../i18n/de/libopietooth.ts
TRANSLATIONS += ../../../../i18n/en/libopietooth.ts
TRANSLATIONS += ../../../../i18n/es/libopietooth.ts
TRANSLATIONS += ../../../../i18n/fr/libopietooth.ts
TRANSLATIONS += ../../../../i18n/hu/libopietooth.ts
TRANSLATIONS += ../../../../i18n/ja/libopietooth.ts
TRANSLATIONS += ../../../../i18n/ko/libopietooth.ts
TRANSLATIONS += ../../../../i18n/no/libopietooth.ts
TRANSLATIONS += ../../../../i18n/pl/libopietooth.ts
TRANSLATIONS += ../../../../i18n/pt/libopietooth.ts
TRANSLATIONS += ../../../../i18n/pt_BR/libopietooth.ts
TRANSLATIONS += ../../../../i18n/sl/libopietooth.ts
TRANSLATIONS += ../../../../i18n/zh_CN/libopietooth.ts
TRANSLATIONS += ../../../../i18n/zh_TW/libopietooth.ts

