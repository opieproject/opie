TEMPLATE     = app
#CONFIG       = qt warn_on release
CONFIG 	     = qt debug
#DESTDIR      = $(OPIEDIR)/bin
HEADERS      = ../io_layer.h ../io_serial.h \
	    senderui.h ../profile.h
SOURCES      = ../io_layer.cpp ../io_serial.cpp \
               ../profile.cpp \
               main.cpp senderui.cpp   
INTERFACES   = sender.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopie
TARGET       = test

