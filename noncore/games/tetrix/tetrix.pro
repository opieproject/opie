TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= gtetrix.h \
		    qtetrix.h \
		    qtetrixb.h \
		    tpiece.h \
			ohighscoredlg.h
SOURCES		= main.cpp \
		    gtetrix.cpp \
		    qtetrix.cpp \
		    qtetrixb.cpp \
		    tpiece.cpp \
			ohighscoredlg.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lstdc++
INTERFACES	= 
TARGET		= tetrix

include ( $(OPIEDIR)/include.pro )
