CONFIG          += qt warn_on  quick-app
HEADERS		= d3des.h \
		  krfbbuffer.h \
		  krfbcanvas.h \
		  krfbconnection.h \
		  krfbdecoder.h \
		  krfblogin.h \
		  krfbserver.h \
		  krfbserverinfo.h \
		  kvnc.h \
		  kvncconndlg.h \
		  kvncbookmarkdlg.h \		  
		  vncauth.h
SOURCES		= d3des.c \
		  vncauth.c \
		  krfbbuffer.cpp \
		  krfbcanvas.cpp \
		  krfbconnection.cpp \
		  krfbdecoder.cpp \
		  krfblogin.cpp \
		  krfbserver.cpp \
		  kvnc.cpp \
		  kvncconndlg.cpp \
		  kvncbookmarkdlg.cpp \
		  main.cpp
INTERFACES	= kvncconndlgbase.ui \
						kvncbookmarkdlgbase.ui

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe

TARGET		= keypebble

include ( $(OPIEDIR)/include.pro )
