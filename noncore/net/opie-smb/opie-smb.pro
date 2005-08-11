TEMPLATE	= app
CONFIG		+= qte warn_on quickapp debug
HEADERS		= qsmb.h
SOURCES		= main.cpp qsmb.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
DESTDIR = $(OPIEDIR)/bin
LIBS   += -lqpe -lopiecore2 -lopieui2 -lpthread

# INCLUDEPATH	+= $(QPEDIR)/include
# DEPENDPATH	+= $(QPEDIR)/include
# LIBS            += 

INTERFACES	= qsmbbase.ui
TARGET		= opie-smb

#DEFINES +=  QT_QWS_NONOPIE
include( $(OPIEDIR)/include.pro )
