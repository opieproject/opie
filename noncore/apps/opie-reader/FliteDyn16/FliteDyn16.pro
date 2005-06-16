DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= lib
CONFIG		= qt warn_on release dll
HEADERS		= flitedyn.h

SOURCES		= flitedyn.cpp


INTERFACES	= 
DESTDIR		= $(OPIEDIR)/plugins/reader/outcodecs
TARGET		= flitedyn16
LIBS            += -lflite_cmu_us_kal16 -lflite_usenglish -lflite_cmulex -lflite

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include

include( $(OPIEDIR)/include.pro )

!isEmpty( LIBFLITE_INC_DIR ) {
    INCLUDEPATH = $$LIBFLITE_INC_DIR $$INCLUDEPATH
}
!isEmpty( LIBFLITE_LIB_DIR ) {
    LIBS = -L$$LIBFLITE_LIB_DIR $$LIBS
}



