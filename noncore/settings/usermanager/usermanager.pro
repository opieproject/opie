#CONFIG        = qt warn_on 
CONFIG        = qt warn_on  quick-app
HEADERS        = usermanager.h userdialog.h groupdialog.h passwd.h
SOURCES        = usermanager.cpp userdialog.cpp groupdialog.cpp passwd.cpp main.cpp
INCLUDEPATH    += $(OPIEDIR)/include
DEPENDPATH    += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lopieui2 -lcrypt
TARGET        = usermanager

include ( $(OPIEDIR)/include.pro )
