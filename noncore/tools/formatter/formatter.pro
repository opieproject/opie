CONFIG      += qt warn_on quick-app
HEADERS     = formatter.h inputDialog.h output.h
SOURCES     = formatter.cpp inputDialog.cpp output.cpp main.cpp
TARGET      = formatter
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2

include ( $(OPIEDIR)/include.pro )
