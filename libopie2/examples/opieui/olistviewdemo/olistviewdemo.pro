TEMPLATE        = app
CONFIG          = qt warn_on debug
HEADERS         = olistviewdemo.h
SOURCES         = olistviewdemo.cpp \
                  main.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lopieui2 -lopiecore2
TARGET          = olistviewdemo
MOC_DIR     = moc
OBJECTS_DIR = obj

include ( $(OPIEDIR)/include.pro )


