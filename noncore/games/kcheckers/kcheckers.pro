TEMPLATE	= app
CONFIG		= qt warn_on release
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
INCLUDEPATH	+= . \
		  $(OPIEDIR)/include
LIBS		+= -lqpe
DESTDIR		= $(OPIEDIR)/bin
TARGET		= kcheckers

TRANSLATIONS = ../../i18n/pt_BR/kcheckers.ts
