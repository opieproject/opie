TEMPLATE        = app
CONFIG          = qt warn_on 
HEADERS         =
SOURCES         = osoundsystemdemo.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lopiecore2 -lopiemm2
TARGET          = osoundsystemdemo
MOC_DIR     = moc
OBJECTS_DIR = obj

include ( $(OPIEDIR)/include.pro )
