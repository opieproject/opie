TEMPLATE    = lib
CONFIG      += qt warn_on debug
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = omanufacturerdb.cpp \
              onetutils.cpp       \
              onetwork.h          \
              opcap.h
SOURCES     = omanufacturerdb.cpp \
              onetutils.cpp       \
              onetwork.cpp        \
              opcap.cpp
INTERFACES  =
TARGET      = opienet2
VERSION     = 1.8.1
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lpcap
MOC_DIR     = moc
OBJECTS_DIR = obj

include ( $(OPIEDIR)/include.pro )

