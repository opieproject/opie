TEMPLATE        = lib
CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/plugins/networksettings2
HEADERS		= wlan_NN.h \
                  wlan_NNI.h \
                  wlanedit.h
SOURCES		= wlan_NN.cpp \
                  wlan_NNI.cpp \
                  wlanedit.cpp \
                  wlanrun.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../networksettings2
DEPENDPATH	+= $(OPIEDIR)/include ../ ../networksettings2
LIBS            += -lqpe
INTERFACES	= wlanGUI.ui
TARGET		= wlan
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
