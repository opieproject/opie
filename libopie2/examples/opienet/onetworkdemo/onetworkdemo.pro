TEMPLATE        = app
CONFIG          = qt warn_on 
HEADERS         =
SOURCES         = onetworkdemo.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lopiecore2 -lopienet2
TARGET          = onetworkdemo
MOC_DIR     = moc
OBJECTS_DIR = obj

include ( $(OPIEDIR)/include.pro )
