TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= kprocctrl.h kprocess.h
SOURCES		= kprocctrl.cpp kprocess.cpp
TARGET		= opietooth
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR         = $(QTDIR)/lib$(PROJMAK)
#VERSION = 0.0.0
