CONFIG += qt 

TEMPLATE = app
SOURCES = ../../../../noncore/graphics/opie-eye/lib/okeyconfigwidget.cpp testwidget.cpp
HEADERS = ../../../../noncore/graphics/opie-eye/lib/okeyconfigwidget.h testwidget.h

INCLUDEPATH += $(OPIEDIR)/include $(OPIEDIR)/noncore/graphics/opie-eye/lib/
DESTPATH += $(OPIEDIR)/include

LIBS += -lopieui2 -lopiecore2

include ( $(OPIEDIR)/include.pro )
