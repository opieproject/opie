TEMPLATE     = app
CONFIG       = qt warn_on release
DESTDIR      = $(OPIEDIR)/bin
HEADERS      = io_layer.h io_serial.h \
	       file_layer.h \
	       metafactory.h \
	       session.h \
	       mainwindow.h
SOURCES      = io_layer.cpp io_serial.cpp \
	       file_layer.cpp main.cpp \
	       metafactory.cpp \
	       session.cpp \
	       mainwindow.cpp
INTERFACES   = 
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopie
TARGET       = opie-console

