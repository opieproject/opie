TEMPLATE        = app
CONFIG          = qt warn_on 
HEADERS         =
SOURCES         = oprocessdemo.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lopiecore2
TARGET          = oprocessdemo

include ( $(OPIEDIR)/include.pro )


