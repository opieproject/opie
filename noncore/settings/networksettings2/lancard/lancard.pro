TEMPLATE        = lib
CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/plugins/networksettings2
HEADERS		= lancard_NN.h \
                  lancard_NNI.h \
                  lancardedit.h
SOURCES		= lancard_NN.cpp \
                  lancard_NNI.cpp \
                  lancardedit.cpp \
                  lancardrun.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../networksettings2
DEPENDPATH	+= $(OPIEDIR)/include ../ ../networksettings2
LIBS            += -lqpe
INTERFACES	= lancardGUI.ui
TARGET		= lancard
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
