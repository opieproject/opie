CONFIG    += qt warn_on release quick-app
HEADERS   = opieftp.h inputDialog.h 
SOURCES   = opieftp.cpp inputDialog.cpp main.cpp
TARGET    = opieftp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lftplib

include ( $(OPIEDIR)/include.pro )
