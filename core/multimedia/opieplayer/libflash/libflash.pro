TEMPLATE	=   lib
CONFIG		+=  qt warn_on release
HEADERS		=   libflashplugin.h   libflashpluginimpl.h
SOURCES		=   libflashplugin.cpp libflashpluginimpl.cpp \
		    adpcm.cc character.cc flash.cc graphic16.cc matrix.cc script.cc \
		    sprite.cc bitmap.cc cxform.cc font.cc graphic24.cc movie.cc \
		    shape.cc sqrt.cc button.cc displaylist.cc graphic.cc graphic32.cc \
		    program.cc sound.cc text.cc
TARGET		=   flashplugin
DESTDIR		=   ../../plugins/codecs
INCLUDEPATH += $(OPIEDIR)/include ..
DEPENDPATH      +=  ../$(OPIEDIR)/include ..
LIBS            +=  -lqpe
VERSION		=   1.0.0

TRANSLATIONS = ../../../../i18n/de/libflashplugin.ts \
	 ../../../../i18n/en/libflashplugin.ts \
	 ../../../../i18n/es/libflashplugin.ts \
	 ../../../../i18n/fr/libflashplugin.ts \
	 ../../../../i18n/hu/libflashplugin.ts \
	 ../../../../i18n/ja/libflashplugin.ts \
	 ../../../../i18n/ko/libflashplugin.ts \
	 ../../../../i18n/no/libflashplugin.ts \
	 ../../../../i18n/pl/libflashplugin.ts \
	 ../../../../i18n/pt/libflashplugin.ts \
	 ../../../../i18n/pt_BR/libflashplugin.ts \
	 ../../../../i18n/sl/libflashplugin.ts \
	 ../../../../i18n/zh_CN/libflashplugin.ts \
	 ../../../../i18n/zh_TW/libflashplugin.ts
