#
CONFIG   = qt warn_on debug
#CONFIG    = qt warn_on release
HEADERS   = networksettings.h \
            activateprofile.h \
            editconnection.h
SOURCES   = main.cpp \
            networksettings.cpp \
            nsdata.cpp \
            activateprofile.cpp \
            editconnection.cpp
INCLUDEPATH += $(OPIEDIR)/include networksettings2/
DEPENDPATH  += $(OPIEDIR)/include networksettings2/
LIBS        += -lqpe -L$(OPIEDIR)/plugins/networksettings2 -lnetworksettings2 -lopie
INTERFACES = networksettingsGUI.ui \
             editconnectionGUI.ui \
             activateprofileGUI.ui
TARGET    = $(OPIEDIR)/bin/networksettings2


include ( $(OPIEDIR)/include.pro )
