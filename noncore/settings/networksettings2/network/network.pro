TEMPLATE        = lib
CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/plugins/networksettings2
HEADERS		= network_NN.h \
                  network_NNI.h \
                  networkedit.h
SOURCES		= network_NN.cpp \
                  network_NNI.cpp \
                  networkedit.cpp \
                  networkrun.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../networksettings2
DEPENDPATH	+= $(OPIEDIR)/include ../ ../networksettings2
LIBS            += -lqpe
INTERFACES	= networkGUI.ui
TARGET		= network
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
