TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   pickboard.h pickboardcfg.h pickboardimpl.h pickboardpicks.h
SOURCES	=   pickboard.cpp pickboardcfg.cpp pickboardimpl.cpp pickboardpicks.cpp
TARGET		= qpickboard
DESTDIR		= ../../plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ../../launcher
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS = ../../i18n/de/libqpickboard.ts \
	 ../../i18n/en/libqpickboard.ts \
	 ../../i18n/es/libqpickboard.ts \
	 ../../i18n/fr/libqpickboard.ts \
	 ../../i18n/hu/libqpickboard.ts \
	 ../../i18n/ja/libqpickboard.ts \
	 ../../i18n/ko/libqpickboard.ts \
	 ../../i18n/no/libqpickboard.ts \
	 ../../i18n/pl/libqpickboard.ts \
	 ../../i18n/pt/libqpickboard.ts \
	 ../../i18n/pt_BR/libqpickboard.ts \
	 ../../i18n/sl/libqpickboard.ts \
	 ../../i18n/zh_CN/libqpickboard.ts \
	 ../../i18n/zh_TW/libqpickboard.ts
