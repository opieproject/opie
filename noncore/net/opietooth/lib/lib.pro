TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= connection.h parser.h device.h manager.h remotedevice.h services.h
SOURCES		= connection.cpp parser.cc device.cc manager.cc remotedevice.cc services.cc
TARGET		= opietooth
INCLUDEPATH += $(OPIEDIR)/include .
DESTDIR         = $(QTDIR)/lib$(PROJMAK)
LIBS += -lopie
#VERSION = 0.0.0

TRANSLATIONS = ../../../../i18n/de/libopietooth.ts \
	 ../../../../i18n/en/libopietooth.ts \
	 ../../../../i18n/es/libopietooth.ts \
	 ../../../../i18n/fr/libopietooth.ts \
	 ../../../../i18n/hu/libopietooth.ts \
	 ../../../../i18n/ja/libopietooth.ts \
	 ../../../../i18n/ko/libopietooth.ts \
	 ../../../../i18n/no/libopietooth.ts \
	 ../../../../i18n/pl/libopietooth.ts \
	 ../../../../i18n/pt/libopietooth.ts \
	 ../../../../i18n/pt_BR/libopietooth.ts \
	 ../../../../i18n/sl/libopietooth.ts \
	 ../../../../i18n/zh_CN/libopietooth.ts \
	 ../../../../i18n/zh_TW/libopietooth.ts
