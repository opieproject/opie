TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= kprocctrl.h kprocess.h device.h manager.h remotedevice.h services.h
SOURCES		= kprocctrl.cpp kprocess.cpp device.cc manager.cc remotedevice.cc services.cc
TARGET		= opietooth
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR         = $(QTDIR)/lib$(PROJMAK)
#VERSION = 0.0.0
