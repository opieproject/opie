TEMPLATE	= app
CONFIG		+= console warn_on 
CONFIG		-= qt
DESTDIR		= $(OPIEDIR)/bin
SOURCES		= main.cpp
TARGET		= pyquicklauncher


include ( $(OPIEDIR)/include.pro )
