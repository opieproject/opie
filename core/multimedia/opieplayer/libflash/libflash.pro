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

