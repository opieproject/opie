TEMPLATE 	= lib
CONFIG   	+=  qt warn_on release
HEADERS   	= libmad_version.h fixed.h bit.h timer.h stream.h frame.h synth.h decoder.h \
		layer12.h layer3.h huffman.h libmad_global.h mad.h libmadplugin.h libmadpluginimpl.h
SOURCES   	= version.c fixed.c bit.c timer.c stream.c frame.c synth.c decoder.c \
        	layer12.c layer3.c huffman.c libmadplugin.cpp libmadpluginimpl.cpp
TARGET   	= madplugin
DESTDIR   	= $(OPIEDIR)/plugins/codecs
INCLUDEPATH += $(OPIEDIR)/include ..
DEPENDPATH      += ../$(OPIEDIR)/include ..
LIBS            += -lqpe -lm
VERSION   =   1.0.0

TRANSLATIONS = ../../../../i18n/de/libmadplugin.ts \
	 ../../../../i18n/en/libmadplugin.ts \
	 ../../../../i18n/es/libmadplugin.ts \
	 ../../../../i18n/fr/libmadplugin.ts \
	 ../../../../i18n/hu/libmadplugin.ts \
	 ../../../../i18n/ja/libmadplugin.ts \
	 ../../../../i18n/ko/libmadplugin.ts \
	 ../../../../i18n/no/libmadplugin.ts \
	 ../../../../i18n/pl/libmadplugin.ts \
	 ../../../../i18n/pt/libmadplugin.ts \
	 ../../../../i18n/pt_BR/libmadplugin.ts \
	 ../../../../i18n/sl/libmadplugin.ts \
	 ../../../../i18n/zh_CN/libmadplugin.ts \
	 ../../../../i18n/zh_TW/libmadplugin.ts
