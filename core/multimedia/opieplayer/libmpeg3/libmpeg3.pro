QMAKE_CFLAGS += $(if $(CONFIG_TARGET_X86),-DHAVE_MMX -DHAVE_CSS) \
	$(if $(CONFIG_TARGET_IPAQ),-DUSE_FIXED_POINT) \
	$(if $(CONFIG_TARGET_SHARP),-DUSE_FIXED_POINT)
QMAKE_CC = $(CXX)
TEMPLATE  =   lib
CONFIG    +=  qt warn_on release
HEADERS   =   libmpeg3plugin.h   libmpeg3pluginimpl.h
SOURCES   =   $(if $(CONFIG_TARGET_X86),video/mmxidct.S video/reconmmx.s) libmpeg3plugin.cpp libmpeg3pluginimpl.cpp \
        bitstream.c \
        libmpeg3.c \
        mpeg3atrack.c \
        mpeg3css.c \
        mpeg3demux.c \
        mpeg3io.c \
        mpeg3title.c \
        mpeg3vtrack.c \
        audio/ac3.c \
        audio/bit_allocation.c \
        audio/dct.c \
        audio/exponents.c \
        audio/header.c \
        audio/layer2.c \
        audio/layer3.c \
        audio/mantissa.c \
        audio/mpeg3audio.c \
        audio/pcm.c \
        audio/synthesizers.c \
        audio/tables.c \
        video/getpicture.c \
        video/headers.c \
        video/idct.c \
        video/macroblocks.c \
        video/mmxtest.c \
        video/motion.c \
        video/mpeg3video.c \
        video/output.c \
        video/reconstruct.c \
        video/seek.c \
        video/slice.c \
        video/vlc.c
TARGET    =   mpeg3plugin
TMAKE_CC=$(CXX)
DESTDIR   =   $(OPIEDIR)/plugins/codecs
INCLUDEPATH += $(OPIEDIR)/include ..
DEPENDPATH      +=  ../$(OPIEDIR)/include ..
LIBS            +=  -lqpe -lpthread -lm
VERSION   =   1.0.0

TRANSLATIONS = ../../../../i18n/de/libmpeg3plugin.ts \
	 ../../../../i18n/en/libmpeg3plugin.ts \
	 ../../../../i18n/es/libmpeg3plugin.ts \
	 ../../../../i18n/fr/libmpeg3plugin.ts \
	 ../../../../i18n/hu/libmpeg3plugin.ts \
	 ../../../../i18n/ja/libmpeg3plugin.ts \
	 ../../../../i18n/ko/libmpeg3plugin.ts \
	 ../../../../i18n/no/libmpeg3plugin.ts \
	 ../../../../i18n/pl/libmpeg3plugin.ts \
	 ../../../../i18n/pt/libmpeg3plugin.ts \
	 ../../../../i18n/pt_BR/libmpeg3plugin.ts \
	 ../../../../i18n/sl/libmpeg3plugin.ts \
	 ../../../../i18n/zh_CN/libmpeg3plugin.ts \
	 ../../../../i18n/zh_TW/libmpeg3plugin.ts
