CONFIG      += qt warn_on  
HEADERS     = opieftp.h inputDialog.h
SOURCES     = opieftp.cpp inputDialog.cpp main.cpp
TARGET      = opieftp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lftplib
DESTDIR = $(OPIEDIR)/bin

include ( $(OPIEDIR)/include.pro )
