TEMPLATE	= lib
CONFIG		+= qt plugin warn_on release
HEADERS	=   dvorak.h \
	    dvorakimpl.h 
SOURCES	=   dvorak.cpp \
	    dvorakimpl.cpp
TARGET		= qdvorak
DESTDIR		= ../../plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ../../launcher
LIBS            += -lqpe -L$(OPIEDIR)/plugins/inputmethods -lqpickboard
QMAKE_LFLAGS	+= -Wl,-rpath,/opt/QtPalmtop/plugins/inputmethods
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/inputmethods
