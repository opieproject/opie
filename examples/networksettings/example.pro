#TEMPLATE        = app
#
TEMPLATE        = lib
#CONFIG          += qt plugin warn_on 
CONFIG		+= qt plugin warn_on 
DESTDIR         = $(OPIEDIR)/plugins/networksettings
HEADERS		= exampleiface.h examplemodule.h 
SOURCES		= exampleiface.cpp examplemodule.cpp
INCLUDEPATH	+= $(OPIEDIR)/include $(OPIEDIR)/noncore/settings/networksettings  $(OPIEDIR)/noncore/settings/networksettings/interfaces
DEPENDPATH	+= $(OPIEDIR)/include $(OPIEDIR)/noncore/settings/networksettings  $(OPIEDIR)/noncore/settings/networksettings/interfaces
LIBS            += -lqpe -linterfaces
TARGET		= example_vpn
VERSION		= 1.0.0



include ( $(OPIEDIR)/include.pro )
