TEMPLATE	=   lib
CONFIG		+=  qt warn_on release
HEADERS		=   wavplugin.h wavpluginimpl.h
SOURCES	        =   wavplugin.cpp wavpluginimpl.cpp
TARGET		=   wavplugin
DESTDIR		=   $(OPIEDIR)/plugins/codecs
INCLUDEPATH += $(OPIEDIR)/include ..
DEPENDPATH      +=  ../$(OPIEDIR)/include ..
LIBS            +=  -lqpe
VERSION		=   1.0.0

TRANSLATIONS = ../../../../i18n/de/libwavplugin.ts \
	 ../../../../i18n/en/libwavplugin.ts \
	 ../../../../i18n/es/libwavplugin.ts \
	 ../../../../i18n/fr/libwavplugin.ts \
	 ../../../../i18n/hu/libwavplugin.ts \
	 ../../../../i18n/ja/libwavplugin.ts \
	 ../../../../i18n/ko/libwavplugin.ts \
	 ../../../../i18n/no/libwavplugin.ts \
	 ../../../../i18n/pl/libwavplugin.ts \
	 ../../../../i18n/pt/libwavplugin.ts \
	 ../../../../i18n/pt_BR/libwavplugin.ts \
	 ../../../../i18n/sl/libwavplugin.ts \
	 ../../../../i18n/zh_CN/libwavplugin.ts \
	 ../../../../i18n/zh_TW/libwavplugin.ts
