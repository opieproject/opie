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

include ( $(OPIEDIR)/include.pro )
	
DEFINES += FPM_INTEL

system(echo $QMAKESPEC | grep -s sharp) {
	DEFINES -= FPM_INTEL
	DEFINES += FPM_ARM
}

system(echo $QMAKESPEC | grep -s ipaq) {
	DEFINES -= FPM_INTEL
	DEFINES += FPM_ARM
}

system(echo $QMAKESPEC | grep -s mipsel) {
	DEFINES -= FPM_INTEL
	DEFINES += FPM_MIPS
}

system(echo $QMAKESPEC | grep -s ramses) {
	DEFINES -= FPM_INTEL
	DEFINES += FPM_ARM
}

system(echo $QMAKESPEC | grep -s arm) {
	DEFINES -= FPM_INTEL
	DEFINES += FPM_ARM
}

system(echo $QMAKESPEC | grep -s simpad) {
	DEFINES -= FPM_INTEL
	DEFINES += FPM_ARM
}

system(echo $QMAKESPEC | grep -s yopy) {
	DEFINES -= FPM_INTEL
	DEFINES += FPM_ARM
}


