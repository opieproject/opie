TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= drawmode.h \
		  drawpad.h \
		  drawpadcanvas.h \
		  ellipsedrawmode.h \
		  erasedrawmode.h \
		  filldrawmode.h \
		  linedrawmode.h \
		  pointdrawmode.h \
		  rectangledrawmode.h
SOURCES		= main.cpp \
		  drawmode.cpp \
		  drawpad.cpp \
		  drawpadcanvas.cpp \
		  ellipsedrawmode.cpp \
		  erasedrawmode.cpp \
		  filldrawmode.cpp \
		  linedrawmode.cpp \
		  pointdrawmode.cpp \
		  rectangledrawmode.cpp
INCLUDEPATH	+= $(OPIEDIR)/include \
                   $(QTDIR)/src/3rdparty/zlib
DEPENDPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe
DESTDIR		= $(OPIEDIR)/bin
TARGET		= drawpad
