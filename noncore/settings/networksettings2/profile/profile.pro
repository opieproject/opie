TEMPLATE        = lib
CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/plugins/networksettings2
HEADERS		= profile_NN.h \
                  profile_NNI.h \
                  profileedit.h
SOURCES		= profile_NN.cpp \
                  profile_NNI.cpp \
                  profileedit.cpp \
                  profilerun.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../networksettings2
DEPENDPATH	+= $(OPIEDIR)/include ../ ../networksettings2
LIBS            += -lqpe
INTERFACES	= profileGUI.ui
TARGET		= profile
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
