TEMPLATE        = lib
CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/plugins/networksettings2
HEADERS		= cable_NN.h \
                  cable_NNI.h \
                  cableedit.h
SOURCES		= cable_NN.cpp \
                  cable_NNI.cpp \
                  cableedit.cpp \
                  cablerun.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../networksettings2
DEPENDPATH	+= $(OPIEDIR)/include ../ ../networksettings2
LIBS            += -lqpe
INTERFACES	= cableGUI.ui
TARGET		= cable
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
