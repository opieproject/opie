TEMPLATE	= lib
CONFIG		+=	qt plugin warn_on release
HEADERS		+=	mailapplet.h \
			mailappletimpl.h 
SOURCES		+=	mailapplet.cpp \
			mailappletimpl.cpp 
INCLUDEPATH += $(OPIEDIR)/include 
LIBS		+=	-lmailwrapper -lqpe -lopie
TARGET		=	mailapplet
DESTDIR		+=	$(OPIEDIR)/plugins/applets/

include ( $(OPIEDIR)/include.pro )
