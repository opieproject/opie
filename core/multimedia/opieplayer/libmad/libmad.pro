TEMPLATE 	= lib
CONFIG   	+=  qt warn_on 
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

contains(QMAKE_CXX, -DQT_QWS_IPAQ) | contains(QMAKE_CXX, -DQT_QWS_SHARP) {
DEFINES += FPM_ARM
} else {
DEFINES += FPM_INTEL
}

include ( $(OPIEDIR)/include.pro )
