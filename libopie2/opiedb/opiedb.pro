TEMPLATE    = lib
CONFIG      += qt warn_on debug
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = 
              
SOURCES     =
INTERFACES  =
TARGET      = opiedb2
VERSION     = 1.8.1
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

MOC_DIR     = moc
OBJECTS_DIR = obj

include ( $(OPIEDIR)/include.pro )

