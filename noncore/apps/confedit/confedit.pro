CONFIG        = qt warn_on release quick-app
HEADERS        = mainwindow.h listviewconfdir.h listviewitemconffile.h listviewitemconfigentry.h editwidget.h listviewitemconf.h
SOURCES        = main.cpp mainwindow.cpp listviewconfdir.cpp listviewitemconffile.cpp listviewitemconfigentry.cpp editwidget.cpp listviewitemconf.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH    += $(OPIEDIR)/include
INTERFACES    =
LIBS            += -lopiecore2
TARGET        = confedit

!contains( platform, x11 ) {

  include ( $(OPIEDIR)/include.pro )
  LIBS += -lqpe
}

contains( platform, x11 ) {
  LIBS        += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
}



