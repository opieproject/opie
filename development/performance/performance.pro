CONFIG    += qt quick-app
HEADERS   = performancetest.h
SOURCES   = main.cpp performancetest.cpp
INTERFACES  = performancetestbase.ui
TARGET    = performance
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lqte

include ( $(OPIEDIR)/include.pro )
