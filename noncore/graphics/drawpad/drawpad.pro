CONFIG		= qt warn_on  quick-app
HEADERS		= drawpad.h \
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
		  page.h \
		  pageinformationdialog.h \
		  pointtool.h \
		  rectangletool.h \
		  shapetool.h \
		  texttool.h \
		  thumbnailview.h \
		  tool.h
SOURCES		= drawpad.cpp \
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
		  page.cpp \
		  pageinformationdialog.cpp \
		  pointtool.cpp \
		  rectangletool.cpp \
		  shapetool.cpp \
		  texttool.cpp \
		  thumbnailview.cpp \
		  tool.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe -lopiecore2 -lopieui2 -lqtaux2

TARGET		= drawpad

include ( $(OPIEDIR)/include.pro )
