TEMPLATE        = lib
CONFIG          += qt warn_on release
#CONFIG		+= qt warn_on debug
DESTDIR		= $(OPIEDIR)/lib$(PROJMAK)
HEADERS		= netnode.h  \
                  resources.h  \
                  system.h  \
                  asline.h \
                  GUIUtils.h \
                  asconnection.h \
                  asfullsetup.h \
                  systemfile.h \
                  asdevice.h
SOURCES		= netnode.cpp \
                  GUIUtils.cpp \
                  system.cpp \
                  systemfile.cpp \
                  resources.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../networksettings2
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie
INTERFACES	= 
TARGET		= networksettings2
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
