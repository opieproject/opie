CONFIG		+= qt warn_on  quick-app
HEADERS		= light.h sensor.h calibration.h
SOURCES		= light.cpp main.cpp sensor.cpp calibration.cpp
INTERFACES	= lightsettingsbase.ui sensorbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= ../$(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2

TARGET		= light-and-power

include ( $(OPIEDIR)/include.pro )
