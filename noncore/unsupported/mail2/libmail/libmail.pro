TEMPLATE	=	lib
CONFIG		+=	qt warn_on debug
#CONFIG		+=	qt warn_on release
HEADERS		=	configfile.h \
			defines.h \
			imapbase.h \
			imaphandler.h \
			imapresponse.h \
			mailfactory.h \
			md5.h \
			miscfunctions.h \
			smtphandler.h
SOURCES		=	configfile.cpp \
			imapbase.cpp \
			imaphandler.cpp \
			imapresponse.cpp \
			mailfactory.cpp \
			md5.cpp \
			miscfunctions.cpp \
			smtphandler.cpp
INCLUDEPATH	+=	$(OPIEDIR)/include
LIBS		+=	-L$(OPIEDIR)/lib -lqpe
TARGET		=	coremail
DESTDIR      = $(OPIEDIR)/lib

include ( $(OPIEDIR)/include.pro )
