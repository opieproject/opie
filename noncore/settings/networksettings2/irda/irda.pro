TEMPLATE        = lib
CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/plugins/networksettings2
HEADERS		= irda_NN.h \
                  irda_NNI.h \
                  irdaedit.h
SOURCES		= irda_NN.cpp \
                  irda_NNI.cpp \
                  irdaedit.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../networksettings2
DEPENDPATH	+= $(OPIEDIR)/include ../ ../networksettings2
LIBS            += -lqpe 
INTERFACES	= irdaGUI.ui
TARGET		= irda
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
