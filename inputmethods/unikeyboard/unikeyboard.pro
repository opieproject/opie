TEMPLATE	= lib
CONFIG		+= qt plugin warn_on release
HEADERS	=   unikeyboard.h unikeyboardimpl.h
SOURCES	=   unikeyboard.cpp unikeyboardimpl.cpp
TARGET		= qunikeyboard
DESTDIR		= ../../plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      +=  ../../launcher
LIBS            += -lqpe
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/inputmethods
