#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release quick-app
HEADERS		= usermanager.h userdialog.h groupdialog.h passwd.h
SOURCES		= usermanager.cpp userdialog.cpp groupdialog.cpp passwd.cpp main.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie -lcrypt
TARGET		= usermanager

include ( $(OPIEDIR)/include.pro )
