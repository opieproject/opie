TEMPLATE        = lib
CONFIG          += qt warn_on release
#CONFIG		+= qt warn_on debug
DESTDIR		= $(OPIEDIR)/lib$(PROJMAK)
HEADERS		= interface.h  interfaceinformationimp.h  interfaces.h  interfacesetupimp.h
SOURCES		= interface.cpp interfaces.cpp interfaceinformationimp.cpp  interfacesetupimp.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie
INTERFACES	= interfaceadvanced.ui  interfaceinformation.ui  interfacesetup.ui
TARGET		= interfaces
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
