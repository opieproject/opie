CONFIG        = qt warn_on quick-app
HEADERS       = mainwindow.h listviewconfdir.h listviewitemconffile.h listviewitemconfigentry.h editwidget.h listviewitemconf.h
SOURCES       = main.cpp mainwindow.cpp listviewconfdir.cpp listviewitemconffile.cpp listviewitemconfigentry.cpp editwidget.cpp listviewitemconf.cpp
INCLUDEPATH   += $(OPIEDIR)/include
DEPENDPATH    += $(OPIEDIR)/include
LIBS          += -lqpe -lopiecore2
TARGET        = confedit

include ( $(OPIEDIR)/include.pro )
