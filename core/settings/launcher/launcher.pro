TEMPLATE     = app
CONFIG      += qt warn_on release
DESTDIR      = $(OPIEDIR)/bin
HEADERS      = launchersettings.h \
               tabssettings.h \
               taskbarsettings.h

SOURCES      = main.cpp \
               launchersettings.cpp \
               tabssettings.cpp \
               taskbarsettings.cpp

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopie
TARGET       = launchersettings

