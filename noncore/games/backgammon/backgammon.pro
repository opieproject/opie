CONFIG		= qt warn_on release quick-app

HEADERS   = backgammon.h \
            backgammonview.h \
            canvasimageitem.h \
            themedialog.h \
            moveengine.h \
            filedialog.h \
            playerdialog.h \
            aidialog.h \
            rulesdialog.h \
            definition.h

SOURCES   = main.cpp \
            backgammon.cpp \
            backgammonview.cpp \
            canvasimageitem.cpp \
            themedialog.cpp \
            moveengine.cpp \
            filedialog.cpp \
            playerdialog.cpp \
            aidialog.cpp \
            rulesdialog.cpp \
            definition.cpp

TARGET 		= backgammon
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe -lopiecore2 -lstdc++


include ( $(OPIEDIR)/include.pro )
