TEMPLATE	= app
CONFIG		= qt warn_on debug

HEADERS		= powerchord.h   \
                  fretboard.h    \
                  chordengine.h  \
                  vumeter.h      \
                  gs.h gt.h      \
                  powerchordbase.h

SOURCES		= main.cpp        \
                  powerchord.cpp  \
                  fretboard.cpp   \
                  chordengine.cpp \
                  vumeter.cpp     \
                  gs.cpp gt.cpp   \
                  powerchordbase.cpp

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
#INTERFACES	= powerchordbase.ui
DESTDIR         = $(OPIEDIR)/bin
TARGET		= powerchord

include ( $(OPIEDIR)/include.pro )

