TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= colordialog.h \
		  colorpanel.h \
		  drawpad.h \
		  drawpadcanvas.h \
		  ellipsetool.h \
		  erasetool.h \
		  exportdialog.h \
		  filltool.h \
		  filledellipsetool.h \
		  filledrectangletool.h \
		  importdialog.h \
		  linetool.h \
		  newpagedialog.h \
		  pointtool.h \
		  rectangletool.h \
		  shapetool.h \
		  tool.h
SOURCES		= colordialog.cpp \
		  colorpanel.cpp \
		  drawpad.cpp \
		  drawpadcanvas.cpp \
		  ellipsetool.cpp \
		  erasetool.cpp \
		  exportdialog.cpp \
		  filltool.cpp \
		  filledellipsetool.cpp \
		  filledrectangletool.cpp \
		  importdialog.cpp \
		  linetool.cpp \
		  main.cpp \
		  newpagedialog.cpp \
		  pointtool.cpp \
		  rectangletool.cpp \
		  shapetool.cpp \
		  tool.cpp
INCLUDEPATH	+= $(OPIEDIR)/include \
		  $(QTDIR)/src/3rdparty/zlib
DEPENDPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe
DESTDIR		= $(OPIEDIR)/bin
TARGET		= drawpad
