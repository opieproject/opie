TEMPLATE	= lib
CONFIG		+= qt plugin warn_on 
HEADERS	=   exampleboardimpl.h
SOURCES	=   exampleboardimpl.cpp
TARGET		= example_board
DESTDIR		= $(OPIEDIR)/plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      +=  ../../launcher
LIBS            += -lqpe
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
