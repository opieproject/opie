TEMPLATE        = lib
CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/plugins/networksettings2
HEADERS		= bluetooth_NN.h \
                  bluetoothBNEP_NNI.h \
                  bluetoothRFCOMM_NNI.h \
                  bluetoothBNEPedit.h \
                  bluetoothRFCOMMedit.h
SOURCES		= bluetooth_NN.cpp \
                  bluetoothBNEP_NNI.cpp \
                  bluetoothRFCOMM_NNI.cpp \
                  bluetoothBNEPedit.cpp \
                  bluetoothRFCOMMedit.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../networksettings2
DEPENDPATH	+= $(OPIEDIR)/include ../ ../networksettings2
LIBS            += -lqpe
INTERFACES	= bluetoothBNEPGUI.ui bluetoothRFCOMMGUI.ui
TARGET		= bluetooth
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
