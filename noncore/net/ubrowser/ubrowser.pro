TEMPLATE	=app
CONFIG		+=qt warn_on release
DESTDIR		=../../../bin
HEADERS		= mainview.h httpfactory.h
SOURCES		= main.cpp mainview.cpp httpfactory.cpp
INCLUDEPATH	+=../../../include
DEPENDPATH	+=../../../include
LIBS		+= -lqpe
TARGET          = ubrowser



include ( $(OPIEDIR)/include.pro )
