CONFIG = qt
TEMPLATE = app
TARGET = opie-eye_slave
DESTDIR = $(OPIEDIR)/bin

HEADERS = gif_slave.h slaveiface.h slavereciever.h \
	  thumbnailtool.h png_slave.h jpeg_slave.h \
	  ../iface/slaveiface.h
SOURCES = main.cpp gif_slave.cpp   slavereciever.cpp \
	  slaveiface.cpp thumbnailtool.cpp png_slave.cpp \
	  jpeg_slave.cpp

INCLUDEPATH += $(OPIEDIR)/include ../
DEPENDSPATH += $(OPIEDIR)/include

LIBS += -lqpe -lopiecore2

include ( $(OPIEDIR)/include.pro )
