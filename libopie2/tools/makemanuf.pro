DESTDIR         = .
TEMPLATE        = app
CONFIG          = qt warn_on 

HEADERS         =

SOURCES         = makemanuf.cpp

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
INTERFACES      =
LIBS            += -lopiecore2 -lopienet2
TARGET          = makemanuf

include ( $(OPIEDIR)/include.pro )


