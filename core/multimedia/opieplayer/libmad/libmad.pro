QMAKE_CFLAGS += $(if $(CONFIG_TARGET_X86),-DFPM_INTEL) \
		$(if $(CONFIG_TARGET_IPAQ),-DFPM_ARM) \
		$(if $(CONFIG_TARGET_SHARP),-DFPM_ARM)
QMAKE_CXXFLAGS += $(if $(CONFIG_TARGET_X86),-DFPM_INTEL) \
		$(if $(CONFIG_TARGET_IPAQ),-DFPM_ARM) \
		$(if $(CONFIG_TARGET_SHARP),-DFPM_ARM)
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

include ( $(OPIEDIR)/include.pro )
