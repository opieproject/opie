TEMPLATE  = app
CONFIG    += qte quick-app warn_on 
HEADERS   = bartender.h newdrinks.h showdrinks.h inputDialog.h searchresults.h bac.h drinkdata.h
SOURCES   = main.cpp bartender.cpp newdrinks.cpp showdrinks.cpp inputDialog.cpp searchresults.cpp bac.cpp drinkdata.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lopieui2
DESTDIR     = $(OPIEDIR)/bin
TARGET      = bartender

include( $(OPIEDIR)/include.pro )
