#TEMPLATE        = app
TEMPLATE        = lib
#CONFIG          += qt warn_on release
CONFIG		+= qt warn_on debug
DESTDIR         = $(OPIEDIR)/plugins/networksettings
HEADERS		= pppimp.h pppmodule.h devices.h modem.h modeminfo.h pppdata.h kpppconfig.h pppdata.h runtests.h
SOURCES		= pppimp.cpp pppmodule.cpp modem.cpp modeminfo.cpp pppdata.cpp runtests.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../interfaces/
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -L../interfaces/ -linterfaces
INTERFACES	= ppp.ui
TARGET		= pppplugin
VERSION		= 1.0.0



include ( $(OPIEDIR)/include.pro )
