TEMPLATE    = app
DESTDIR         = $(OPIEDIR)/bin/
#CONFIG        = qt warn_on 
CONFIG        = qt warn_on  quick-app
HEADERS        = mediumwidget.h mediumglobal.h mainwindow.h
SOURCES        = main.cpp mediumwidget.cc mediumglobal.cc mainwindow.cc
INCLUDEPATH    += $(OPIEDIR)/include
DEPENDPATH    += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
TARGET          = mediummount

include ( $(OPIEDIR)/include.pro )
