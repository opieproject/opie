CONFIG		= qt warn_on release quick-app
HEADERS		= wordgame.h
SOURCES		= main.cpp \
		  wordgame.cpp
INTERFACES	= newgamebase.ui rulesbase.ui
TARGET		= wordgame
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe


include ( $(OPIEDIR)/include.pro )
