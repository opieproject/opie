TEMPLATE     = app
#CONFIG       = qt warn_on release
CONFIG 	     = qt debug
#DESTDIR      = $(OPIEDIR)/bin
HEADERS      = ../io_layer.h ../io_serial.h ../sz_transfer.h ../file_layer.h\
	    senderui.h ../profile.h ../filetransfer.h ../procctl.h \
	    ../filereceive.h ../receive_layer.h
SOURCES      = ../io_layer.cpp ../io_serial.cpp \
               ../profile.cpp ../sz_transfer.cpp ../file_layer.cpp\
               main.cpp senderui.cpp   ../filetransfer.cpp ../procctl.cpp \
	       ../filereceive.cpp ../receive_layer.cpp 
INTERFACES   = sender.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopie
TARGET       = test

