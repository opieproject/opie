TEMPLATE	= app
CONFIG		= qt warn_on debug
# CONFIG		= qt warn_on release
#HEADERS		=
SOURCES		=  converter.cpp
INTERFACES      =  converter_base.ui
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie 
TARGET		= converter

include ( $(OPIEDIR)/include.pro )
