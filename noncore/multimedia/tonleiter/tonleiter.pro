TEMPLATE  = app
#CONFIG   = qt warn_on debug
CONFIG    = qt warn_on release
HEADERS   = mainwidget.h tonleiterdata.h tonleiterdatahelper.h fretboard.h menuwidget.h
SOURCES   = mainwidget.cpp tonleiterdata.cpp tonleiterdatahelper.cpp fretboard.cpp menuwidget.cpp main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe 
INTERFACES  =
TARGET    = tonleiter
DESTDIR = $(OPIEDIR)/bin
#TMAKE_CXXFLAGS += -DQT_QWS_VERCEL_IDR -DQWS -fno-exceptions -fno-rtti
include ( $(OPIEDIR)/include.pro )
