TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= colordialog.h \
		  colorpanel.h \
		  drawmode.h \
		  drawpad.h \
		  drawpadcanvas.h \
		  ellipsedrawmode.h \
		  erasedrawmode.h \
		  filldrawmode.h \
		  linedrawmode.h \
		  newpagedialog.h \
		  pointdrawmode.h \
		  rectangledrawmode.h \
		  shapedrawmode.h
SOURCES		= colordialog.cpp \
		  colorpanel.cpp \
		  drawmode.cpp \
		  drawpad.cpp \
		  drawpadcanvas.cpp \
		  ellipsedrawmode.cpp \
		  erasedrawmode.cpp \
		  filldrawmode.cpp \
		  linedrawmode.cpp \
		  main.cpp \
		  newpagedialog.cpp \
		  pointdrawmode.cpp \
		  rectangledrawmode.cpp \
		  shapedrawmode.cpp
INCLUDEPATH	+= $(OPIEDIR)/include \
		  $(QTDIR)/src/3rdparty/zlib
DEPENDPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe
DESTDIR		= $(OPIEDIR)/bin
TARGET		= drawpad
