CONFIG        += qt warn_on  quick-app
HEADERS        = doctab.h
SOURCES        = doctab.cpp main.cpp
INTERFACES    = doctabsettingsbase.ui
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH    += 
LIBS            += -lqpe -lopiecore2
TARGET        = doctab

include ( $(OPIEDIR)/include.pro )
