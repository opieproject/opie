TEMPLATE        = lib
CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/plugins/networksettings2
HEADERS		= usb_NN.h \
                  usb_NNI.h \
                  usbedit.h
SOURCES		= usb_NN.cpp \
                  usb_NNI.cpp \
                  usbedit.cpp \
                  usbrun.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../networksettings2
DEPENDPATH	+= $(OPIEDIR)/include ../ ../networksettings2
LIBS            += -lqpe
INTERFACES	= usbGUI.ui
TARGET		= usb
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
