TEMPLATE	= app
CONFIG		+= qt warn_on console
HEADERS		= ../shared/metatranslator.h \
		  ../shared/proparser.h \
		  ../shared/opie.h
SOURCES		= main.cpp \
		  ../shared/metatranslator.cpp \
		  ../shared/proparser.cpp \
		  ../shared/opie.cpp

DEFINES 	+= QT_INTERNAL_XML

TARGET		= opie-lrelease
INCLUDEPATH	+= ../shared
#DESTDIR		= ../../../bin

