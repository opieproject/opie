TEMPLATE	=   lib
CONFIG		+=  qt warn_on release
HEADERS		=   libmpeg3plugin.h   libmpeg3pluginimpl.h
SOURCES		=   libmpeg3plugin.cpp libmpeg3pluginimpl.cpp \
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
TARGET		=   mpeg3plugin
DESTDIR		=   ../../plugins/codecs
INCLUDEPATH += $(OPIEDIR)/include ..
DEPENDPATH      +=  ../$(OPIEDIR)/include ..
LIBS            +=  -lqpe -lpthread -lm
VERSION		=   1.0.0
