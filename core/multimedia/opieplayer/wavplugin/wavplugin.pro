TEMPLATE	=   lib
CONFIG		+=  qt plugin warn_on 
HEADERS		=   wavplugin.h wavpluginimpl.h
SOURCES	        =   wavplugin.cpp wavpluginimpl.cpp
TARGET		=   wavplugin
DESTDIR		=   $(OPIEDIR)/plugins/codecs
INCLUDEPATH += $(OPIEDIR)/include ..
DEPENDPATH      +=  ../$(OPIEDIR)/include ..
LIBS            +=  -lqpe
VERSION		=   1.0.0

include ( $(OPIEDIR)/include.pro )
