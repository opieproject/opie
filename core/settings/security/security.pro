CONFIG      += qt warn_on  quick-app
HEADERS     = security.h
SOURCES     = security.cpp main.cpp
INTERFACES  = securitybase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += 
LIBS            += -lqpe -lopiecore2
TARGET      = security

include ( $(OPIEDIR)/include.pro )
