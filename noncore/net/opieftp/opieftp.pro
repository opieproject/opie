TEMPLATE  = app
CONFIG      += qte  quick-app warn_on 
HEADERS     = opieftp.h inputDialog.h
SOURCES     = opieftp.cpp inputDialog.cpp main.cpp
TARGET      = opieftp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lftplib

include( $(OPIEDIR)/include.pro )
