CONFIG += qt 

TEMPLATE = app
SOURCES = testwidget.cpp
HEADERS = testwidget.h

INCLUDEPATH += $(OPIEDIR)/include $(OPIEDIR)/noncore/graphics/opie-eye/lib/
DESTPATH += $(OPIEDIR)/include

LIBS += -lopieui2 -lopiecore2

include ( $(OPIEDIR)/include.pro )
