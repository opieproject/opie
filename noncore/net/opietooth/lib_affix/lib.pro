TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= manager.h \
                  startpanconnection.h \
                  startdunconnection.h 
SOURCES		= manager.cc \
                  startpanconnection.cpp \
                  startdunconnection.cpp
TARGET		= opietooth
INCLUDEPATH += $(OPIEDIR)/include ../include
DESTDIR		= $(OPIEDIR)/plugins/opietooth
LIBS += -lopie -lopietooth_common \
        /usr/lib/libaffix.so /usr/lib/libaffix_utils.so /usr/lib/libaffix_sdp.so
#VERSION = 0.0.0

TRANSLATIONS = ../../../../i18n/de/libopietooth.ts \
	 ../../../../i18n/da/libopietooth.ts \
	 ../../../../i18n/xx/libopietooth.ts \
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



include ( $(OPIEDIR)/include.pro )
