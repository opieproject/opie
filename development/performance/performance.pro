TEMPLATE  = app
CONFIG    += qt warn_on release
#DESTDIR    = $(QPEDIR)/bin
HEADERS   = performancetest.h
SOURCES   = main.cpp performancetest.cpp
INTERFACES  = performancetestbase.ui
TARGET    = performance
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lqte

include ( $(OPIEDIR)/include.pro )
