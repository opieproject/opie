TEMPLATE        = app
CONFIG          = qt warn_on debug
HEADERS         =
SOURCES         = odebugdemo.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lopiecore2
TARGET          = odebugdemo
MOC_DIR     = moc
OBJECTS_DIR = obj

include ( $(OPIEDIR)/include.pro )


