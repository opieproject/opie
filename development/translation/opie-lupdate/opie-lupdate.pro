TEMPLATE	= app
CONFIG		+= qt warn_on console
HEADERS		= ../shared/metatranslator.h \
		  ../shared/proparser.h \
		  ../shared/opie.h
		  
SOURCES		= fetchtr.cpp \
		  main.cpp \
		  merge.cpp \
		  numberh.cpp \
		  sametexth.cpp \
		  ../shared/metatranslator.cpp \
		  ../shared/proparser.cpp \
		  ../shared/opie.cpp

DEFINES 	+= QT_INTERNAL_XML

TARGET		= opie-lupdate
INCLUDEPATH	+= ../shared
#DESTDIR		= 

