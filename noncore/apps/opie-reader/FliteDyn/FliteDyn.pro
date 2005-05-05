DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
HEADERS		= flitedyn.h

SOURCES		= flitedyn.cpp


INTERFACES	= 
DESTDIR		= $(OPIEDIR)/plugins/reader/outcodecs
TARGET		= flitedyn
LIBS            += -L/home/tim/flite/flite-1.2-release/lib -lflite_cmu_us_kal -lflite_usenglish -lflite_cmulex -lflite

INCLUDEPATH	+= /home/tim/flite/flite-1.2-release/include $(OPIEDIR)/include
DEPENDPATH += /home/tim/flite/flite-1.2-release/include $(OPIEDIR)/include

include( $(OPIEDIR)/include.pro )
