TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
#TMAKE_CXXFLAGS	+= 
HEADERS		= kpacmanwidget.h \
		    referee.h \
		    status.h \
		    painter.h \
		    score.h \
		    pacman.h \
		    monster.h \
		    keys.h \
		    fruit.h \
		    energizer.h \
		    board.h \
		    bitfont.h \
		    kpacman.h \
		    bitmaps.h \
		    colors.h \
		    config.h \
		    portable.h
SOURCES		= kpacmanwidget.cpp \
		    referee.cpp \
		    status.cpp \
		    painter.cpp \
		    score.cpp \
		    pacman.cpp \
		    monster.cpp \
		    keys.cpp \
		    fruit.cpp \
		    energizer.cpp \
		    board.cpp \
		    bitfont.cpp \
		    kpacman.cpp \
		    config.cpp \
		    main.cpp 

#INCLUDEPATH	+= 
#DEPENDPATH	+=
LIBS            += -lqpe -ljpeg
DESTDIR         = $(OPIEDIR)/bin
TARGET		= kpacman
