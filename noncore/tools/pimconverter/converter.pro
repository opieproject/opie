CONFIG		= qt quick-app
HEADERS         =  converter.h
SOURCES		=  converter.cpp
INTERFACES      =  converter_base.ui
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lopiepim2 -lopiedb2 
TARGET		= opimconverter

include ( $(OPIEDIR)/include.pro )
