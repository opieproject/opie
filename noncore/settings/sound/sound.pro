CONFIG    += qt warn_on release quick-app
HEADERS   = soundsettings.h soundsettingsbase.h
SOURCES   = soundsettings.cpp soundsettingsbase.cpp main.cpp
#INTERFACES = soundsettingsbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
TARGET    = sound

include ( $(OPIEDIR)/include.pro )
