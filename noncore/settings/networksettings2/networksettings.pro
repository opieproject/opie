#
CONFIG   = qt warn_on debug quick-app
#CONFIG    = qt warn_on release
HEADERS   = networksettings.h \
            editconnection.h
SOURCES   = main.cpp \
            networksettings.cpp \
            editconnection.cpp
INCLUDEPATH += $(OPIEDIR)/include networksettings2/
DEPENDPATH  += $(OPIEDIR)/include networksettings2/
LIBS        += -lqpe -L$(OPIEDIR)/plugins/networksettings2 -lnetworksettings2 -lopie
INTERFACES = networksettingsGUI.ui editconnectionGUI.ui
TARGET    = networksettings2


include ( $(OPIEDIR)/include.pro )
