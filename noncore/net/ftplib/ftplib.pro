TEMPLATE  = lib
CONFIG    = qt warn_on 
HEADERS   = ftplib.h
SOURCES   = ftplib.c
DESTDIR      = $(OPIEDIR)/lib$(PROJMAK)
INTERFACES  = 
TARGET    = ftplib
QMAKE_LINK=$$QMAKE_CC

include( $(OPIEDIR)/include.pro )
