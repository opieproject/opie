CONFIG		= qt warn_on release quick-app
HEADERS		= calculatorimpl.h
SOURCES		= calculatorimpl.cpp \
		  main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= calculator.ui
TARGET		= calculator

include ( $(OPIEDIR)/include.pro )
