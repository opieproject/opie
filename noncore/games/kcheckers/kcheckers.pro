CONFIG		= qt warn_on release quick-app
HEADERS		= checkers.h \
		  echeckers.h \
		  field.h \
		  kcheckers.h \
		  rcheckers.h
SOURCES		= checkers.cpp \
		  echeckers.cpp \
		  field.cpp \
		  kcheckers.cpp \
		  main.cpp \
		  rcheckers.cpp
INTERFACES	= 
INCLUDEPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe
TARGET		= kcheckers


include ( $(OPIEDIR)/include.pro )
