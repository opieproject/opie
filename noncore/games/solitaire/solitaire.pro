
CONFIG    += qt warn_on release quick-app

HEADERS   = canvascard.h canvasshapes.h cardgame.h cardgamelayout.h cardpile.h card.h carddeck.h \
    canvascardgame.h freecellcardgame.h chicanecardgame.h harpcardgame.h teeclubcardgame.h \
    patiencecardgame.h canvascardwindow.h

SOURCES   = canvascard.cpp canvasshapes.cpp cardgame.cpp cardgamelayout.cpp \
   cardpile.cpp card.cpp carddeck.cpp canvascardgame.cpp freecellcardgame.cpp \
   chicanecardgame.cpp harpcardgame.cpp teeclubcardgame.cpp \
   patiencecardgame.cpp canvascardwindow.cpp main.cpp

TARGET    = patience

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2


include ( $(OPIEDIR)/include.pro )
