DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
HEADERS		= Aportis.h \
		  CExpander.h \
		  ../OREADERINC/useqpe.h \
		  config.h \
		  ustring.h \
		  StyleConsts.h \
		  Markups.h \
		  names.h \
		  linktype.h \
		  pdb.h \
		  my_list.h \
		  Bkmks.h \
		  Filedata.h

SOURCES		= Aportis.cpp


INTERFACES	= 
DESTDIR		= $(OPIEDIR)/plugins/reader/codecs
TARGET		= Aportis
LIBS            += -lreader_pdb -lreader_codec

INCLUDEPATH	+= ../OREADERINC $(OPIEDIR)/include
DEPENDPATH	+= ../OREADERINC $(OPIEDIR)/include

include ( $(OPIEDIR)/include.pro )