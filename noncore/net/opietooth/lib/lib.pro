TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= kprocctrl.h kprocess.h device.h
SOURCES		= kprocctrl.cpp kprocess.cpp device.cc
TARGET		= opietooth
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR         = $(QTDIR)/lib$(PROJMAK)
#VERSION = 0.0.0
