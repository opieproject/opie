#
TEMPLATE        = lib
#TEMPLATE        = app
CONFIG          += qt plugin warn_on 
#CONFIG		+= qt plugin warn_on 
DESTDIR         = $(OPIEDIR)/plugins/networksettings
HEADERS		= infoimp.h wlanmodule.h wextensions.h keyedit.h
SOURCES		= infoimp.cpp wlanmodule.cpp wextensions.cpp keyedit.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../interfaces/
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -L../interfaces/ -linterfaces -lopiecore2 -lopienet2
INTERFACES	= wlan.ui info.ui
TARGET		= wlan
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
