TEMPLATE        = app
CONFIG          = qt warn_on 
HEADERS         = opieuidemo.h \
                  oversatileviewdemo.h
SOURCES         = opieuidemo.cpp \
                  oversatileviewdemo.cpp \
                  main.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lopieui2 -lopiecore2
TARGET          = opieuidemo
MOC_DIR     = moc
OBJECTS_DIR = obj

include ( $(OPIEDIR)/include.pro )


