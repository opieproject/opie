TEMPLATE  = app
#CONFIG   = qt warn_on debug
CONFIG    = qt warn_on release
HEADERS   = bartender.h newdrinks.h showdrinks.h inputDialog.h searchresults.h bac.h
SOURCES   = main.cpp bartender.cpp newdrinks.cpp showdrinks.cpp inputDialog.cpp searchresults.cpp bac.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe
DESTDIR = $(OPIEDIR)/bin
TARGET    = bartender

include ( $(OPIEDIR)/include.pro )
