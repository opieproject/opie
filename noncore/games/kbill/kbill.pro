TEMPLATE 	= app
CONFIG		= qt warn_on release
HEADERS 	= kbill.h field.h Bucket.h Cable.h Computer.h Game.h Horde.h \
Library.h MCursor.h Monster.h Network.h Picture.h Spark.h Strings.h \
UI.h  objects.h inputbox.h
SOURCES		= field.cpp Bucket.cc Cable.cc Computer.cc Game.cc Horde.cc \
Library.cc MCursor.cc Monster.cc  Network.cc Picture.cc \ 
Spark.cc UI.cpp inputbox.cpp kbill.cpp
TARGET 		= kbill
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe
DESTDIR 	=  $(OPIEDIR)/bin