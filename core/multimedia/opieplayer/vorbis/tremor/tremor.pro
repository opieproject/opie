TEMPLATE  = lib
CONFIG    = qt warn_on release
HEADERS   = asm_arm.h \
      backends.h \
      codebook.h \
      codec_internal.h \
      config_types.h \
      ivorbiscodec.h \
      ivorbisfile.h \
      lsp_lookup.h \
      mdct.h \
      mdct_lookup.h \
      misc.h \
      ogg.h \
      os.h \
      os_types.h \
      registry.h \
      window.h \
      window_lookup.h
SOURCES   = bitwise.c \
      block.c \
      codebook.c \
      floor0.c \
      floor1.c \
      framing.c \
      info.c \
      mapping0.c \
      mdct.c \
      registry.c \
      res012.c \
      sharedbook.c \
      synthesis.c \
      vorbisfile.c \
      window.c
INTERFACES  = 
DESTDIR      = $(OPIEDIR)/lib$(PROJMAK)
#DESTDIR   =   $(OPIEDIR)/root/usr/lib
INCLUDEPATH += $(OPIEDIR)/include ..
DEPENDPATH      +=  $(OPIEDIR)/include ..
#LIBS            +=  -lqpe
VERSION   =   1.0.0

include ( $(OPIEDIR)/include.pro )
