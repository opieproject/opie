TEMPLATE	=	lib
CONFIG		=	qt warn_on debug
#CONFIG		=	qt warn_on release
HEADERS		=	configfile.h \
			defines.h \
			imapbase.h \
			imaphandler.h \
			imapresponse.h \
			mailfactory.h \
			md5.cpp \
			miscfunctions.h \
			sharp_char.h \
			smtphandler.h \
			zaurusstuff.h
SOURCES		=	configfile.cpp \
			imapbase.cpp \
			imaphandler.cpp \
			imapresponse.cpp \
			mailfactory.cpp \
			md5.h \
			miscfunctions.cpp \
			smtphandler.cpp \
			zaurusstuff.cpp
INCLUDEPATH	+=	$(OPIEDIR)/include
LIBS		+=	-L$(OPIEDIR)/lib -lqpe
TARGET		=	mail
DESTDIR		=	$(QTDIR)/lib
