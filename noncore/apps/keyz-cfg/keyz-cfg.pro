TEMPLATE        = app
DESTDIR         = $(OPIEDIR)/bin
CONFIG		= qt warn_on 
HEADERS		= zkb.h \
				zkbcfg.h \
				zkbnames.h \
				zkbxml.h \
				cfgdlg.h \
				cfgfile.h

SOURCES		= main.cpp \
		  cfgdlg.cpp \
		  cfgfile.cpp \
		  zkb.cpp \
		  zkbcfg.cpp \
			zkbnames.cpp \
			zkbxml.cpp

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS		+= -lqpe
TARGET 		= keyz-cfg

include ( $(OPIEDIR)/include.pro )
