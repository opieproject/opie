TEMPLATE        = lib
CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/plugins/networksettings2
HEADERS		= vpn_NN.h \
                  vpn_NNI.h \
                  vpnedit.h
SOURCES		= vpn_NN.cpp \
                  vpn_NNI.cpp \
                  vpnedit.cpp \
                  vpnrun.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../networksettings2
DEPENDPATH	+= $(OPIEDIR)/include ../ ../networksettings2
LIBS            += -lqpe
INTERFACES	= vpnGUI.ui
TARGET		= vpn
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
