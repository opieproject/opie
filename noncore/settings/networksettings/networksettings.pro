CONFIG   = qt warn_on debug quick-app
HEADERS   = mainwindowimp.h addconnectionimp.h defaultmodule.h  module.h
SOURCES   = main.cpp mainwindowimp.cpp addconnectionimp.cpp
INCLUDEPATH += $(OPIEDIR)/include interfaces/
DEPENDPATH  += $(OPIEDIR)/include interfaces/ wlan/ ppp/
LIBS        += -lqpe -L$(OPIEDIR)/plugins/networksettings -Linterfaces/ -linterfaces -lopie -lopiecore2 -lopienet2 
INTERFACES = mainwindow.ui addconnection.ui
TARGET    = networksettings



include ( $(OPIEDIR)/include.pro )
