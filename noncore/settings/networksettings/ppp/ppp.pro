#TEMPLATE        = app
#
TEMPLATE        = lib
#CONFIG          += qt plugin warn_on release
CONFIG		+= qt plugin warn_on debug
DESTDIR         = $(OPIEDIR)/plugins/networksettings
HEADERS		= pppmodule.h  modem.h modeminfo.h pppdata.h kpppconfig.h pppdata.h runtests.h general.h modemcmds.h conwindow.h accounts.h connect.h edit.h scriptedit.h pppdargs.h  iplined.h pwentry.h pppconfig.h interfaceinformationppp.h interfaceppp.h authwidget.h chooserwidget.h devices.h
SOURCES		= pppmodule.cpp modem.cpp modeminfo.cpp pppdata.cpp runtests.cpp general.cpp modemcmds.cpp  conwindow.cpp  accounts.cpp connect.cpp edit.cpp scriptedit.cpp pppdargs.cpp  iplined.cpp pwentry.cpp pppconfig.cpp interfaceinformationppp.cpp interfaceppp.cpp authwidget.cpp chooserwidget.cpp devices.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../interfaces/
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -L../interfaces/ -linterfaces
TARGET		= kppp
VERSION		= 1.0.0



include ( $(OPIEDIR)/include.pro )
