CONFIG += qt warn_on release quick-app
HEADERS = citytime.h citytimebase.h zonemap.h sun.h stylusnormalizer.h
SOURCES = citytime.cpp citytimebase.cpp zonemap.cpp main.cpp sun.c stylusnormalizer.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2

TARGET	= citytime

include ( $(OPIEDIR)/include.pro )
