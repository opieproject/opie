TEMPLATE        = lib
CONFIG          += qt warn_on release plugin
DESTDIR         = $(OPIEDIR)/plugins/networksettings2
HEADERS		= bluetoothBNEP_NN.h \
                  bluetoothBNEP_NNI.h \
                  bluetoothRFCOMM_NN.h \
                  bluetoothRFCOMM_NNI.h \
                  bluetoothBNEPedit.h \
                  bluetoothRFCOMMrun.h \
                  bluetoothRFCOMMedit.h
SOURCES		= bluetoothBNEP_NN.cpp \
                  bluetoothBNEP_NNI.cpp \
                  bluetoothRFCOMM_NN.cpp \
                  bluetoothRFCOMM_NNI.cpp \
                  bluetoothBNEPedit.cpp \
                  bluetoothBNEPrun.cpp \
                  bluetoothRFCOMMrun.cpp \
                  bluetoothRFCOMMedit.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../networksettings2 ../opietooth2
DEPENDPATH	+= $(OPIEDIR)/include ../ ../networksettings2 ../opietooth2
LIBS            += -lqpe -lopietooth2
INTERFACES	= bluetoothBNEPGUI.ui bluetoothRFCOMMGUI.ui
TARGET		= bluetooth
VERSION		= 1.0.0

include( $(OPIEDIR)/include.pro )
