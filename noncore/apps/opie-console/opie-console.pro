TEMPLATE     = app
CONFIG       = qt warn_on release
DESTDIR      = $(OPIEDIR)/bin
HEADERS      = io_layer.h io_serial.h \
	       file_layer.h \
	       metafactory.h \
	       session.h \
	       mainwindow.h \
	       profile.h \
	       profileconfig.h \
	       profilemanager.h  \
	       configwidget.h \
	       tabwidget.h \
	       configdialog.h \
               profileeditordialog.h \
               emulation_layer.h \
               widget.h \
               vt102emulation.h \
               common.h \
               history.h \
               screen.h \
	       keytrans.h

SOURCES      = io_layer.cpp io_serial.cpp \
	       file_layer.cpp main.cpp \
	       metafactory.cpp \
	       session.cpp \
	       mainwindow.cpp \
	       profile.cpp \
	       profileconfig.cpp \
	       profilemanager.cpp  \
	       tabwidget.cpp \
	       configdialog.cpp \
	       profileeditordialog.cpp \
	       emulation_layer.cpp \
	       widget.cpp \
	       vt102emulation.cpp \
	       history.cpp \
	       screen.cpp \
	       keytrans.cpp
   
INTERFACES   = configurebase.ui editbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopie
TARGET       = opie-console

