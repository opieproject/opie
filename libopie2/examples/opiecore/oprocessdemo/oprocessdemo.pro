TEMPLATE        = app
CONFIG          = qt warn_on debug
HEADERS         =
SOURCES         = oprocessdemo.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lopiecore2
TARGET          = oprocessdemo

include ( $(OPIEDIR)/include.pro )


