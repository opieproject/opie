TEMPLATE	= app
CONFIG		+= qt warn_on release

DESTDIR		= $(OPIEDIR)/bin

HEADERS	=	qcleanuphandler.h \
		qcomplextext_p.h \
		qrichtext_p.h \
		qstylesheet.h \
		qtextedit.h \
		mainwindow.h

SOURCES	=	qcomplextext.cpp \
		qstylesheet.cpp \
		qrichtext_p.cpp \
		qrichtext.cpp \
		qtextedit.cpp \
		main.cpp \
		mainwindow.cpp

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

TARGET		= opie-write
