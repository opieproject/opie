CONFIG        += qt warn_on  quick-app
HEADERS        = settings.h
SOURCES        = language.cpp main.cpp
INTERFACES    = languagesettingsbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH    += ../$(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
TARGET        = language

include ( $(OPIEDIR)/include.pro )
