TEMPLATE  = app
CONFIG    = qt warn_on 
HEADERS   = mainwidget.h tonleiterdata.h tonleiterdatahelper.h fretboard.h menuwidget.h editinst.h editstringwidget.h editscale.h pianoscale.h
SOURCES   = mainwidget.cpp tonleiterdata.cpp tonleiterdatahelper.cpp fretboard.cpp menuwidget.cpp main.cpp editinst.cpp editstringwidget.cpp editscale.cpp pianoscale.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 
TARGET    = tonleiter
DESTDIR = $(OPIEDIR)/bin

include ( $(OPIEDIR)/include.pro )
