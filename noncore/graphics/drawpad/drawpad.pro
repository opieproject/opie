CONFIG		= qt warn_on release quick-app
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
LIBS		+= -lqpe -lopie

TARGET		= drawpad

include ( $(OPIEDIR)/include.pro )
