TEMPLATE    = lib
CONFIG      += qt plugin warn_on 
HEADERS     = networkapplet.h
SOURCES     = networkapplet.cpp
TARGET      = networkapplet
DESTDIR     = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lopieui2 -lopienet2
VERSION     = 0.1.0
MOC_DIR     = moc
OBJECTS_DIR = obj


include ( $(OPIEDIR)/include.pro )
