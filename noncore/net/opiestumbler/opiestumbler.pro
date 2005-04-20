TEMPLATE  = app
CONFIG    += qte warn_on quick-app debug
HEADERS   = opiestumbler.h stumblersettings.h stumbler.h \
            stationviewitem.h stumblerstation.h stationinfo.h
SOURCES   = opiestumbler.cpp stumblersettings.cpp stumbler.cpp \
            main.cpp stationviewitem.cpp stumblerstation.cpp \
            stationinfo.cpp
TARGET    = opiestumbler
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lqte -lopiecore2 -lopieui2 -lopienet2

!contains(CONFIG,quick-app) {
 DESTDIR = $(OPIEDIR)/bin
 DEFINES += NOQUICKLAUNCH
}

include( $(OPIEDIR)/include.pro )
