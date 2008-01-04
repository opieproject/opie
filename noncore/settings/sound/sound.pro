CONFIG      += qt warn_on quick-app
HEADERS     = soundsettings.h
SOURCES     = soundsettings.cpp main.cpp
INTERFACES  = soundsettingsbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2
TARGET      = sound

include( $(OPIEDIR)/include.pro )
