TEMPLATE        = lib
CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/plugins/networksettings2
HEADERS		= modem_NN.h \
                  modem_NNI.h \
                  modemedit.h
SOURCES		= modem_NN.cpp \
                  modem_NNI.cpp \
                  modemedit.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../networksettings2
DEPENDPATH	+= $(OPIEDIR)/include ../ ../networksettings2
LIBS            += -lqpe
INTERFACES	= modemGUI.ui
TARGET		= modem
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
