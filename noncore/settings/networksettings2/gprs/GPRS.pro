TEMPLATE        = lib
CONFIG          += qt warn_on release plugin
DESTDIR         = $(OPIEDIR)/plugins/networksettings2
HEADERS		= GPRS_NN.h \
                  GPRS_NNI.h \
                  GPRSedit.h
SOURCES		= GPRS_NN.cpp \
                  GPRS_NNI.cpp \
                  GPRSedit.cpp \
                  GPRSrun.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../networksettings2
DEPENDPATH	+= $(OPIEDIR)/include ../ ../networksettings2
LIBS            += -lqpe
INTERFACES	= GPRSGUI.ui
TARGET		= GPRS
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
