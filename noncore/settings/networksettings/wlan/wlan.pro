TEMPLATE        = lib
CONFIG          += qt warn_on release
#CONFIG		+= qt warn_on debug
DESTDIR         = $(OPIEDIR)/plugins/networksettings
HEADERS		= infoimp.h wlanmodule.h wextensions.h
SOURCES		= infoimp.cpp wlanmodule.cpp wextensions.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../interfaces/
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -L../interfaces/ -linterfaces
INTERFACES	= wlan.ui info.ui
TARGET		= wlanplugin
VERSION		= 1.0.0

#CONFIG += wirelessopts

wirelessopts {
	HEADERS	+= wlanimp.h
	SOURCES	+= wlanimp.cpp
}

! wirelessopts {
	HEADERS	+= wlanimp2.h
	SOURCES += wlanimp2.cpp
}

include ( $(OPIEDIR)/include.pro )
