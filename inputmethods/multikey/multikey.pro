TEMPLATE	= lib
CONFIG		+= qt plugin warn_on release
HEADERS	=   keyboard.h \
            configdlg.h \
	    keyboardimpl.h 
SOURCES	=   keyboard.cpp \
            configdlg.cpp \
	    keyboardimpl.cpp
TARGET		= qmultikey
DESTDIR		= $(OPIEDIR)/plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ../../launcher
LIBS            += -lqpe -L$(OPIEDIR)/plugins/inputmethods -lqpickboard -lqtaux2 -lopieui2
QMAKE_LFLAGS	+= -Wl,-rpath,/opt/QtPalmtop/plugins/inputmethods
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/inputmethods
