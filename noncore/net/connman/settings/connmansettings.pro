CONFIG        += qt warn_on quick-app
HEADERS        = serviceedit.h mainwindowimpl.h service.h technology.h
SOURCES        = serviceedit.cpp mainwindowimpl.cpp main.cpp service.cpp technology.cpp
INTERFACES     = serviceeditbase.ui mainwindow.ui
INCLUDEPATH   += $(OPIEDIR)/include
DEPENDPATH    += $(OPIEDIR)/include
LIBS          += -lqpe -lopiecore2 -ldbus-qt2 $$system(pkg-config --libs dbus-1)
TARGET         = connmansettings

include( $(OPIEDIR)/include.pro )
