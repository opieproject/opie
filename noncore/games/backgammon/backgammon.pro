TEMPLATE 	= app
CONFIG		= qt warn_on release

HEADERS   = backgammon.h \
    canvasimageitem.h \
	themedialog.h \
	moveengine.h \
	filedialog.h \
	playerdialog.h \
	aidialog.h \
	rulesdialog.h \
  displaydialog.h \
  definition.h

SOURCES   = main.cpp \
    backgammon.cpp \
	canvasimageitem.cpp \
	themedialog.cpp \
	moveengine.cpp \
	filedialog.cpp \
	playerdialog.cpp \
	aidialog.cpp \
	rulesdialog.cpp \
  displaydialog.cpp \
  definition.cpp

TARGET 		= backgammon
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe -lstdc++
DESTDIR 	=  $(OPIEDIR)/bin


include ( $(OPIEDIR)/include.pro )
