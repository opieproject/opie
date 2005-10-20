TEMPLATE     = app
CONFIG       = qt warn_on 

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lopieui2 -lopiepim2

HEADERS      = mainwindow.h editwindow.h
SOURCES      = mainwindow.cpp main.cpp editwindow.cpp

include( $(OPIEDIR)/include.pro )
